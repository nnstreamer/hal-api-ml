/* SPDX-License-Identifier: Apache-2.0 */

#include <glib.h>

#include <hal/hal-common.h>

#include "hal-ml-interface.h"
#include "hal-ml.h"

#if defined(__TIZEN__)
#include <dlog.h>
#define LOG_TAG "HAL_ML"
#define _D(fmt, args...)  SLOGD(fmt, ##args)
#define _I(fmt, args...)  SLOGI(fmt, ##args)
#define _W(fmt, args...)  SLOGW(fmt, ##args)
#define _E(fmt, args...)  SLOGE(fmt, ##args)
#else
#include <glib.h>

#define _D g_debug
#define _I g_info
#define _W g_warning
#define _E g_critical
#endif


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

static int hal_ml_create_backend(void **data, void *user_data)
{
  *data = g_new0 (hal_backend_ml_funcs, 1);
  return HAL_ML_ERROR_NONE;
}

static int hal_ml_exit_backend(void *data, void *user_data)
{
  hal_backend_ml_funcs *ml = (hal_backend_ml_funcs *) data;
  g_free (data);
  return HAL_ML_ERROR_NONE;
}

static int hal_ml_backend_count = 0;
static char **hal_ml_backend_names = NULL;

#define MAX_LIB_NAME_LENGTH 256

static int hal_ml_scan_backends(void)
{
  int i;
  _D("Scan available HAL ML backends...");

  hal_ml_backend_count = hal_common_get_backend_count(HAL_MODULE_ML);
  _D("hal_ml_backend_count: %d", hal_ml_backend_count);

  hal_ml_backend_names = (char **) malloc(sizeof (char *) * hal_ml_backend_count);

  for (i = 0; i < hal_ml_backend_count; i++) {
    hal_ml_backend_names[i] = (char *) malloc(sizeof (char) * MAX_LIB_NAME_LENGTH);
  }

  hal_common_get_backend_library_names(HAL_MODULE_ML, hal_ml_backend_names, hal_ml_backend_count, MAX_LIB_NAME_LENGTH);

  for (i = 0; i < hal_ml_backend_count; i++) {
    _D("hal_ml_backend_names[%d]: %s", i, hal_ml_backend_names[i]);
  }

  return 0;
}

int hal_ml_param_create(hal_ml_param_h *param)
{
  if (!param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = g_new0(hal_ml_param_s, 1);
  if (!param_s) {
    return HAL_ML_ERROR_OUT_OF_MEMORY;
  }

  param_s->table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
  if (!param_s->table) {
    g_free (param_s);
    return HAL_ML_ERROR_OUT_OF_MEMORY;
  }

  *param = (hal_ml_param_h) param_s;

  return HAL_ML_ERROR_NONE;
}

int hal_ml_param_destroy(hal_ml_param_h param)
{
  if (param == NULL) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = (hal_ml_param_s *) param;
  if (param_s->table) {
    g_hash_table_destroy(param_s->table);
  }
  g_free (param_s);

  return HAL_ML_ERROR_NONE;
}

int hal_ml_param_set(hal_ml_param_h param, const char *key, void *value)
{
  if (param == NULL || key == NULL || value == NULL) {

    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = (hal_ml_param_s *) param;
  hal_ml_param_value_s *param_value = g_new0(hal_ml_param_value_s, 1);
  if (!param_value) {
    return HAL_ML_ERROR_OUT_OF_MEMORY;
  }

  param_value->value = value;
  g_hash_table_insert(param_s->table, g_strdup(key), (gpointer) param_value);

  return HAL_ML_ERROR_NONE;
}

static int hal_ml_param_get(hal_ml_param_h param, const char *key, void **value)
{
  if (param == NULL || key == NULL || value == NULL) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  hal_ml_param_s *param_s = (hal_ml_param_s *) param;
  hal_ml_param_value_s *param_value = (hal_ml_param_value_s *) g_hash_table_lookup(param_s->table, key);
  if (!param_value) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }

  *value = param_value->value;

  return HAL_ML_ERROR_NONE;
}

int hal_ml_create (const char *backend_name, hal_ml_h *handle)
{
  int i;

  /* scan backend only once */
  static int scanned = 1;
  if (scanned == 1)
    scanned = hal_ml_scan_backends();

  _I("init backend %s", backend_name);

  /* find matched backend */
  for (i = 0; i < hal_ml_backend_count; i++) {
    if (g_strrstr (hal_ml_backend_names[i], backend_name) != NULL) {
      hal_ml_s *new_handle = NULL;
      new_handle = g_new0 (hal_ml_s, 1);
      if (!new_handle) {
        return HAL_ML_ERROR_OUT_OF_MEMORY;
      }

      new_handle->backend_library_name = hal_ml_backend_names[i];

      /* fill functions pointers from backend */
      int ret = hal_common_get_backend_with_library_name_v2(
          HAL_MODULE_ML,
          (void **) &new_handle->funcs,
          NULL,
          hal_ml_create_backend,
          new_handle->backend_library_name);

      if (ret != 0 || !new_handle->funcs || !new_handle->funcs->init) {
        _E("Failed to get backend");
        g_free(new_handle);
        return HAL_ML_ERROR_RUNTIME_ERROR;
      }

      /* init backend */
      ret = new_handle->funcs->init (&new_handle->backend_private);
      if (ret != 0) {
        _E("Failed to initialize backend.");
        hal_common_put_backend_with_library_name_v2(
          HAL_MODULE_ML,
          (void *) new_handle->funcs,
          NULL,
          hal_ml_exit_backend,
          new_handle->backend_library_name);
        return HAL_ML_ERROR_RUNTIME_ERROR;
      }

      *handle = (hal_ml_h) new_handle;
      return ret;
    }
  }

  _E("No backend matched with %s", backend_name);
  return HAL_ML_ERROR_INVALID_PARAMETER;
}

int hal_ml_destroy (hal_ml_h handle)
{
  hal_ml_s *ml = (hal_ml_s *) handle;
  int ret = ml->funcs->deinit (ml->backend_private);

  ret = hal_common_put_backend_with_library_name_v2(
    HAL_MODULE_ML,
    (void *) ml->funcs,
    NULL,
    hal_ml_exit_backend,
    ml->backend_library_name);

  g_free (ml);

  if (ret != 0)
    return HAL_ML_ERROR_RUNTIME_ERROR;

  return HAL_ML_ERROR_NONE;
}

static int _hal_ml_configure_instance(hal_ml_h handle, hal_ml_param_h param) {
  if (!handle || !param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }
  hal_ml_s *ml = (hal_ml_s *) handle;
  const GstTensorFilterProperties *prop;
  hal_ml_param_get(param, "properties", (void **)&prop);
  return ml->funcs->configure_instance(ml->backend_private, prop);
}

static int _hal_ml_invoke(hal_ml_h handle, hal_ml_param_h param) {
  if (!handle || !param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }
  hal_ml_s *ml = (hal_ml_s *) handle;
  const GstTensorMemory *input;
  GstTensorMemory *output;
  hal_ml_param_get(param, "input", (void **)&input);
  hal_ml_param_get(param, "output", (void **)&output);
  return ml->funcs->invoke(ml->backend_private, input, output);
}

static int _hal_ml_invoke_dynamic(hal_ml_h handle, hal_ml_param_h param) {
  if (!handle || !param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }
  hal_ml_s *ml = (hal_ml_s *) handle;
  GstTensorFilterProperties *prop;
  const GstTensorMemory *input;
  GstTensorMemory *output;
  hal_ml_param_get(param, "properties", (void **)&prop);
  hal_ml_param_get(param, "input", (void **)&input);
  hal_ml_param_get(param, "output", (void **)&output);
  return ml->funcs->invoke_dynamic(ml->backend_private, prop, input, output);
}

static int _hal_ml_get_framework_info(hal_ml_h handle, hal_ml_param_h param) {
  if (!handle || !param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }
  hal_ml_s *ml = (hal_ml_s *) handle;
  GstTensorFilterFrameworkInfo *framework_info;
  hal_ml_param_get(param, "framework_info", (void **)&framework_info);
  return ml->funcs->get_framework_info(ml->backend_private, framework_info);
}

static int _hal_ml_get_model_info(hal_ml_h handle, hal_ml_param_h param) {
  if (!handle || !param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }
  hal_ml_s *ml = (hal_ml_s *) handle;
  model_info_ops *ops;
  GstTensorsInfo *in_info;
  GstTensorsInfo *out_info;
  hal_ml_param_get(param, "ops", (void **)&ops);
  hal_ml_param_get(param, "in_info", (void **)&in_info);
  hal_ml_param_get(param, "out_info", (void **)&out_info);
  return ml->funcs->get_model_info(ml->backend_private, *ops, in_info, out_info);
}

static int _hal_ml_event_handler(hal_ml_h handle, hal_ml_param_h param) {
  if (!handle || !param) {
    return HAL_ML_ERROR_INVALID_PARAMETER;
  }
  hal_ml_s *ml = (hal_ml_s *) handle;
  event_ops ops;
  GstTensorFilterFrameworkEventData *data;
  hal_ml_param_get(param, "ops", (void **)&ops);
  hal_ml_param_get(param, "data", (void **)&data);
  return ml->funcs->event_handler(ml->backend_private, ops, data);
}

int hal_ml_request(hal_ml_h handle, const char *request_name, hal_ml_param_h param)
{
  hal_ml_s *ml = (hal_ml_s *) handle;

  if (g_ascii_strcasecmp(request_name, "configure_instance") == 0)
    return _hal_ml_configure_instance(handle, param);

  if (g_ascii_strcasecmp(request_name, "invoke") == 0)
    return _hal_ml_invoke(handle, param);

  if (g_ascii_strcasecmp(request_name, "invoke_dynamic") == 0)
    return _hal_ml_invoke_dynamic(handle, param);

  if (g_ascii_strcasecmp(request_name, "get_framework_info") == 0)
    return _hal_ml_get_framework_info(handle, param);

  if (g_ascii_strcasecmp(request_name, "get_model_info") == 0)
    return _hal_ml_get_model_info(handle, param);

  if (g_ascii_strcasecmp(request_name, "eventHandler") == 0)
    return _hal_ml_event_handler(handle, param);

  _E("Invalid request name %s", request_name);
  return HAL_ML_ERROR_INVALID_PARAMETER;
}

int hal_ml_request_invoke(hal_ml_h handle, const void *input, void *output)
{
  const GstTensorMemory *input_ = (const GstTensorMemory *) input;
  GstTensorMemory *output_ = (GstTensorMemory *) output;

  hal_ml_s *ml = (hal_ml_s *) handle;
  return ml->funcs->invoke(ml->backend_private, input_, output_);
}
