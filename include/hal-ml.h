/**
 * HAL (Hardware Abstract Layer) API for ML
 *
 * Copyright (C) 2025 Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file    hal-ml.h
 * @date    15 Jan 2025
 * @brief   HAL (Hardware Abstract Layer) API for ML
 * @author  Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 * @see     https://github.com/nnstreamer/nnstremaer
 * @bug     No known bugs except for NYI items
 *
 * @details
 *  This HAL ML provides an interface for Tizen ML API (NNStreamer's tensor_filter subplugin)
 *  to use hardware acceleration devices (NPU, ...).
 */

#ifndef __HAL_ML__
#define __HAL_ML__

#include <hal-ml-types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup HAL_ML_MODULE
 * @{
 */

/**
 * @brief A handle for hal-ml instance
 * @since HAL_MODULE_ML 1.0
 */
typedef void *hal_ml_h;

/**
 * @brief A handle for hal-ml-param instance
 * @since HAL_MODULE_ML 1.0
 */
typedef void *hal_ml_param_h;

/**
 * @brief Creates hal-ml-param instance
 * @since HAL_MODULE_ML 1.0
 * @remarks The @a param should be released using hal_ml_param_destroy().
 * @param[out] option Newly created option handle is returned.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 * @retval #HAL_ML_ERROR_OUT_OF_MEMORY Failed to allocate required memory.
 */
int hal_ml_param_create(hal_ml_param_h *param);

/**
 * @brief Destroys hal-ml-param instance
 * @since HAL_MODULE_ML 1.0
 * @param[in] param The handle of the param instance to be destroyed.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 */
int hal_ml_param_destroy(hal_ml_param_h param);

/**
 * @brief Sets a parameter for hal-ml-param instance
 * @since HAL_MODULE_ML 1.0
 * @param[in] param The handle of the param instance.
 * @param[in] key The key of the parameter to set.
 * @param[in] value The value of the parameter to set.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 */
int hal_ml_param_set(hal_ml_param_h param, const char *key, void *value);

/**
 * @brief Creates hal-ml instance
 * @since HAL_MODULE_ML 1.0
 * @remarks The @a handle should be released using hal_ml_destroy().
 * @param[in] backend_name The name of the backend to use.
 * @param[out] handle Newly created handle is returned.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 * @retval #HAL_ML_ERROR_OUT_OF_MEMORY Failed to allocate required memory.
 */
int hal_ml_create(const char *backend_name, hal_ml_h *handle);

/**
 * @brief Destroys hal-ml instance
 * @since HAL_MODULE_ML 1.0
 * @param[in] handle The handle of the instance to be destroyed.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 */
int hal_ml_destroy(hal_ml_h handle);

/**
 * @brief Sends a request to hal-ml instance
 * @since HAL_MODULE_ML 1.0
 * @param[in] handle The handle of the instance.
 * @param[in] request_name The name of the request.
 * @param[in] param The parameters for the request.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 */
int hal_ml_request(hal_ml_h handle, const char *request_name, hal_ml_param_h param);

/**
 * @brief Invokes the hal-ml instance with the given data.
 * @since HAL_MODULE_ML 1.0
 * @param[in] handle The handle of the instance.
 * @param[in] input The input data for the invoke.
 * @param[in, out] output The output data for the invoke.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 */
int hal_ml_request_invoke(hal_ml_h handle, const void *input, void *output);

/**
 * @brief Invoke dynamic the hal-ml instance with the given data.
 * @since HAL_MODULE_ML 1.0
 * @param[in] handle The handle of the instance.
 * @param[in, out] prop The properties for the invoke dynamic.
 * @param[in] input The input data for the invoke.
 * @param[in, out] output The output data for the invoke.
 * @return @c 0 on success. Otherwise a negative error value.
 * @retval #HAL_ML_ERROR_NONE Successful.
 * @retval #HAL_ML_ERROR_INVALID_PARAMETER Fail. The parameter is invalid.
 */
int hal_ml_request_invoke_dynamic(hal_ml_h handle, void *prop, const void *input, void *output);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HAL_ML__ */
