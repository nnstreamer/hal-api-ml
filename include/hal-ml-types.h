#ifndef __HAL_ML_TYPES__
#define __HAL_ML_TYPES__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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


typedef void *hal_ml_h;
typedef void *hal_ml_param_h;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HAL_ML_TYPES__ */
