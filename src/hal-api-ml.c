/**
 * HAL (Hardware Abstract Layer) API for ML
 *
 * Copyright (C) 2025 Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file    hal-api-ml.c
 * @date    15 Jan 2025
 * @brief   HAL (Hardware Abstract Layer) API for ML
 * @author  Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 * @see     https://github.com/nnstreamer/nnstreamer
 * @bug     No known bugs except for NYI items
 *
 * @details
 * This HAL ML provides an interface for Tizen ML API (NNStreamer's
 * tensor_filter subplugin) to use hardware acceleration devices (NPU, ...).
 */

#include <dlog.h>
#include <glib.h>
#include <hal/hal-common.h>
#include "hal-ml-interface.h"
#include "hal-ml.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "HAL_API_ML"
#define _D(fmt, args...) SLOGD (fmt, ##args)
#define _I(fmt, args...) SLOGI (fmt, ##args)
#define _W(fmt, args...) SLOGW (fmt, ##args)
#define _E(fmt, args...) SLOGE (fmt, ##args)

typedef struct _hal_ml_s {
  void *backend_private;
  hal_backend_ml_funcs *funcs;
  gchar *backend_library_name;
} hal_ml_s;

typedef struct _hal_ml_param_s {
  GHashTable *table;
} hal_ml_param_s;

typedef struct _hal_ml_param_value_s {
  void *value;
} hal_ml_param_value_s;

static int
hal_ml_create_backend (void **data, void *user_data)
{
  *data = g_new0 (hal_backend_ml_funcs, 1);
  return HAL_ML_ERROR_NONE;
}

static int
hal_ml_exit_backend (void *data, void *user_data)
{
  g_free (data);
  return HAL_ML_ERROR_NONE;
}

static int hal_ml_backend_count = 0;
static char **hal_ml_backend_names = NULL;

#define MAX_LIB_NAME_LENGTH 256

static int
hal_ml_scan_backends (void)
{
  _D ("Scanning available HAL ML backends...");

  hal_ml_backend_count = hal_common_get_backend_count (HAL_MODULE_ML);
  if (hal_ml_backend_count < 0) {
    _E ("Failed to get backend count");
    return -1;
  }

  if (hal_ml_backend_count == 0) {
    return 0;
  }

  _D ("hal_ml_backend_count: %d", hal_ml_backend_count);

  hal_ml_backend_names = (char **) malloc (sizeof (char *) * hal_ml_backend_count);
  for (int i = 0; i < hal_ml_backend_count; i++) {
    hal_ml_backend_names[i] = (char *) malloc (sizeof (char) * MAX_LIB_NAME_LENGTH);
  }

  hal_common_get_backend_library_names (HAL_MODULE_ML, hal_ml_backend_names,
      hal_ml_backend_count, MAX_LIB_NAME_LENGTH);

  for (int i = 0; i < hal_ml_backend_count; i++) {
    _D ("hal_ml_backend_names[%d]: %s", i, hal_ml_backend_names[i]);
  }

  return 0;
}

int
hal_ml_param_create (hal_ml_param_h *param)
{
  if (!param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = g_new0 (hal_ml_param_s, 1);
  if (!param_s) {
    return HAL_ML_ERROR_OUT_OF_MEMORY;
  }

  param_s->table = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
  if (!param_s->table) {
    g_free (param_s);
    return HAL_ML_ERROR_OUT_OF_MEMORY;
  }

  *param = (hal_ml_param_h) param_s;
  return HAL_ML_ERROR_NONE;
}

int
hal_ml_param_destroy (hal_ml_param_h param)
{
  if (!param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = (hal_ml_param_s *) param;
  if (param_s->table) {
    g_hash_table_destroy (param_s->table);
  }
  g_free (param_s);

  return HAL_ML_ERROR_NONE;
}

int
hal_ml_param_set (hal_ml_param_h param, const char *key, void *value)
{
  if (!param || !key || !value) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = (hal_ml_param_s *) param;
  hal_ml_param_value_s *param_value = g_new0 (hal_ml_param_value_s, 1);
  if (!param_value) {
    return HAL_ML_ERROR_OUT_OF_MEMORY;
  }

  param_value->value = value;
  g_hash_table_insert (param_s->table, g_strdup (key), (gpointer) param_value);

  return HAL_ML_ERROR_NONE;
}

static int
hal_ml_param_get (hal_ml_param_h param, const char *key, void **value)
{
  if (!param || !key || !value) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = (hal_ml_param_s *) param;
  hal_ml_param_value_s *param_value
      = (hal_ml_param_value_s *) g_hash_table_lookup (param_s->table, key);
  if (!param_value) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  *value = param_value->value;
  return HAL_ML_ERROR_NONE;
}

int
hal_ml_create (const char *backend_name, hal_ml_h *handle)
{
  if (!handle) {
    _E ("Got invalid handle");
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  if (!backend_name) {
    _E ("Got invalid backend name");
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  /* Scan backend only once */
  static int scanned = 1;
  if (scanned == 1) {
    scanned = hal_ml_scan_backends ();
    if (scanned < 0) {
      _E ("Failed to scan backends");
      return HAL_ML_ERROR_RUNTIME_ERROR;
    }
  }

  if (hal_ml_backend_count == 0) {
    _E ("There is no available backends");
    return HAL_ML_ERROR_NOT_SUPPORTED;
  }

  _I ("Initializing backend %s", backend_name);

  /* Find matched backend */
  for (int i = 0; i < hal_ml_backend_count; i++) {
    if (g_strrstr (hal_ml_backend_names[i], backend_name) != NULL) {
      hal_ml_s *new_handle = g_new0 (hal_ml_s, 1);
      if (!new_handle) {
        return HAL_ML_ERROR_OUT_OF_MEMORY;
      }

      new_handle->backend_library_name = hal_ml_backend_names[i];

      /* Fill function pointers from backend */
      int ret = hal_common_get_backend_with_library_name_v2 (HAL_MODULE_ML,
          (void **) &new_handle->funcs, NULL, hal_ml_create_backend,
          new_handle->backend_library_name);

      if (ret != 0 || !new_handle->funcs || !new_handle->funcs->init) {
        _E ("Failed to get backend");
        g_free (new_handle);
        return HAL_ML_ERROR_RUNTIME_ERROR;
      }

      /* Initialize backend */
      ret = new_handle->funcs->init (&new_handle->backend_private);
      if (ret != 0) {
        _E ("Failed to initialize backend.");
        hal_common_put_backend_with_library_name_v2 (HAL_MODULE_ML,
            (void *) new_handle->funcs, NULL, hal_ml_exit_backend,
            new_handle->backend_library_name);
        return HAL_ML_ERROR_RUNTIME_ERROR;
      }

      _I ("Backend initialized successfully with %s", new_handle->backend_library_name);
      *handle = (hal_ml_h) new_handle;
      return ret;
    }
  }

  _E ("No backend matched with %s", backend_name);
  return HAL_ML_ERROR_INVALID_PARAMETER;
}

int
hal_ml_destroy (hal_ml_h handle)
{
  hal_ml_s *ml = (hal_ml_s *) handle;

  if (!handle) {
    _E ("Got invalid handle");
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  _I ("Deinitializing backend %s", ml->backend_library_name);

  int ret = ml->funcs->deinit (ml->backend_private);
  if (ret != HAL_ML_ERROR_NONE) {
    _W ("Failed to deinitialize backend.");
  }

  ret = hal_common_put_backend_with_library_name_v2 (HAL_MODULE_ML,
      (void *) ml->funcs, NULL, hal_ml_exit_backend, ml->backend_library_name);

  g_free (ml);

  if (ret != 0) {
    return HAL_ML_ERROR_RUNTIME_ERROR;
  }

  return HAL_ML_ERROR_NONE;
}

static int
_hal_ml_configure_instance (hal_ml_h handle, hal_ml_param_h param)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  const void *prop = NULL;
  hal_ml_param_get (param, "properties", (void **) &prop);
  return ml->funcs->configure_instance (ml->backend_private, prop);
}

static int
_hal_ml_invoke (hal_ml_h handle, hal_ml_param_h param)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  const void *input = NULL;
  void *output = NULL;
  hal_ml_param_get (param, "input", (void **) &input);
  hal_ml_param_get (param, "output", (void **) &output);
  return ml->funcs->invoke (ml->backend_private, input, output);
}

static int
_hal_ml_invoke_dynamic (hal_ml_h handle, hal_ml_param_h param)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  void *prop = NULL;
  const void *input = NULL;
  void *output = NULL;
  hal_ml_param_get (param, "properties", (void **) &prop);
  hal_ml_param_get (param, "input", (void **) &input);
  hal_ml_param_get (param, "output", (void **) &output);
  return ml->funcs->invoke_dynamic (ml->backend_private, prop, input, output);
}

static int
_hal_ml_get_framework_info (hal_ml_h handle, hal_ml_param_h param)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  void *framework_info = NULL;
  hal_ml_param_get (param, "framework_info", (void **) &framework_info);
  return ml->funcs->get_framework_info (ml->backend_private, framework_info);
}

static int
_hal_ml_get_model_info (hal_ml_h handle, hal_ml_param_h param)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  int *model_info_ops = NULL;
  void *in_info = NULL;
  void *out_info = NULL;
  hal_ml_param_get (param, "ops", (void **) &model_info_ops);
  hal_ml_param_get (param, "in_info", (void **) &in_info);
  hal_ml_param_get (param, "out_info", (void **) &out_info);
  return ml->funcs->get_model_info (ml->backend_private, *model_info_ops, in_info, out_info);
}

static int
_hal_ml_event_handler (hal_ml_h handle, hal_ml_param_h param)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  int *event_ops = NULL;
  void *data = NULL;
  hal_ml_param_get (param, "ops", (void **) &event_ops);
  hal_ml_param_get (param, "data", (void **) &data);
  return ml->funcs->event_handler (ml->backend_private, *event_ops, data);
}

int
hal_ml_request (hal_ml_h handle, const char *request_name, hal_ml_param_h param)
{
  if (!handle || !param) {
    _E ("Got invalid parameter");
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  if (g_ascii_strcasecmp (request_name, "configure_instance") == 0)
    return _hal_ml_configure_instance (handle, param);

  if (g_ascii_strcasecmp (request_name, "invoke") == 0)
    return _hal_ml_invoke (handle, param);

  if (g_ascii_strcasecmp (request_name, "invoke_dynamic") == 0)
    return _hal_ml_invoke_dynamic (handle, param);

  if (g_ascii_strcasecmp (request_name, "get_framework_info") == 0)
    return _hal_ml_get_framework_info (handle, param);

  if (g_ascii_strcasecmp (request_name, "get_model_info") == 0)
    return _hal_ml_get_model_info (handle, param);

  if (g_ascii_strcasecmp (request_name, "eventHandler") == 0)
    return _hal_ml_event_handler (handle, param);

  _E ("Invalid request name %s", request_name);
  return HAL_ML_ERROR_INVALID_PARAMETER;
}

int
hal_ml_request_invoke (hal_ml_h handle, const void *input, void *output)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  if (G_UNLIKELY (!handle)) {
    _E ("Got invalid handle");
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  return ml->funcs->invoke (ml->backend_private, input, output);
}
