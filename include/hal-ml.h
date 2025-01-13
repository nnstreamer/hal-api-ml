#include "hal-ml-types.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef void *hal_ml_h;
typedef void *hal_ml_param_h;
int hal_ml_param_create(hal_ml_param_h *param);
int hal_ml_param_destroy(hal_ml_param_h param);
int hal_ml_param_set(hal_ml_param_h param, const char *key, void *value);
// int hal_ml_param_get(hal_ml_param_h param, const char *key, void **value);

int hal_ml_create(const char *backend_name, hal_ml_h *handle);
int hal_ml_destroy(hal_ml_h handle);
int hal_ml_request(hal_ml_h handle, const char *request_name, hal_ml_param_h param);

int hal_ml_request_invoke(hal_ml_h handle, const void *input, void *output);
int hal_ml_request_invoke_dynamic(hal_ml_h handle, void *prop, const void *input, void *output);

#ifdef __cplusplus
}
#endif