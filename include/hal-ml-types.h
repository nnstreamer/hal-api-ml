/**
 * Define types for HAL ML
 *
 * Copyright (C) 2025 Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file    hal-ml-types.h
 * @date    15 Jan 2025
 * @brief   Define types for HAL ML
 * @author  Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 * @see     https://github.com/nnstreamer/nnstremaer
 * @bug     No known bugs except for NYI items
 */

#ifndef __HAL_ML_TYPES__
#define __HAL_ML_TYPES__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup HAL_ML_MODULE
 * @{
 */

/**
 * @brief Enumeration for the error codes of hal-ml
 * @since HAL_MODULE_ML 1.0
 */
typedef enum hal_ml_error {
  HAL_ML_ERROR_NONE = 0,
  HAL_ML_ERROR_INVALID_PARAMETER = -1,
  HAL_ML_ERROR_NOT_SUPPORTED = -2,
  HAL_ML_ERROR_UNKNOWN = -3,
  HAL_ML_ERROR_OUT_OF_MEMORY = -4,
  HAL_ML_ERROR_PERMISSION_DENIED = -5,
  HAL_ML_ERROR_IO_ERROR = -6,
  HAL_ML_ERROR_RUNTIME_ERROR = -7,
} hal_ml_error_e;

/**
 * @}
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HAL_ML_TYPES__ */
