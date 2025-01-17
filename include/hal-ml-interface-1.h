/**
 * HAL (Hardware Abstract Layer) interface API for ML
 *
 * Copyright (C) 2025 Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file    hal-ml-interface-1.h
 * @date    15 Jan 2025
 * @brief   HAL (Hardware Abstract Layer) interface API for ML
 * @author  Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 * @see     https://github.com/nnstreamer/nnstremaer
 * @bug     No known bugs except for NYI items
 *
 * @details
 *    This is the header file for the HAL ML interface version 1.
 */

#ifndef __HAL_ML_INTERFACE_1__
#define __HAL_ML_INTERFACE_1__

#include "hal-ml-types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup HALAPI_HAL_ML_MODULE
 * @{
 */

/**
 * @brief The HAL backend structure for ML. This should be implemented by each HAL backend.
 * @since HAL_MODULE_ML 1.0
 */
typedef struct _hal_backend_ml_funcs {
  /**< Initialize HL backend handle */
  int (*init)(void **backend_private);
  /**< Deinitialize HL backend handle */
  int (*deinit)(void *backend_private);

  /**< Configure ml framework */
  int (*configure_instance)(void *backend_private, const void *prop);
  /**< Invoke */
  int (*invoke)(void *backend_private, const void *input, void *output);
  /**< Invoke dynamic */
  int (*invoke_dynamic)(void *backend_private, void *prop, const void *input, void *output);
  /**< Get framework info */
  int (*get_framework_info)(void *backend_private, void *framework_info);
  /**< Get model info */
  int (*get_model_info)(void *backend_private, int ops, void *in_info, void *out_info);
  /**< Handle event */
  int (*event_handler)(void *backend_private, int ops, void *data);
} hal_backend_ml_funcs;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HAL_ML_INTERFACE_1__ */
