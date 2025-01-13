#include "tensor_typedef.h"
#include "nnstreamer_plugin_api_filter.h"

#include "hal-ml-types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _hal_backend_ml_funcs {
  int (*init)(void **backend_private);
  int (*deinit)(void *backend_private);

  int (*configure_instance)(void *backend_private, const GstTensorFilterProperties *prop);
  int (*invoke)(void *backend_private, const GstTensorMemory *input, GstTensorMemory *output);
  int (*invoke_dynamic)(void *backend_private, GstTensorFilterProperties *prop, const GstTensorMemory *input, GstTensorMemory *output);
  int (*get_framework_info)(void *backend_private, GstTensorFilterFrameworkInfo *framework_info);
  int (*get_model_info)(void *backend_private, model_info_ops ops, GstTensorsInfo *in_info, GstTensorsInfo *out_info);
  int (*event_handler)(void *backend_private, event_ops ops, GstTensorFilterFrameworkEventData *data);
} hal_backend_ml_funcs;

#ifdef __cplusplus
}
#endif
