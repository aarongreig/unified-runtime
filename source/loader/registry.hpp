#include "ur_util.hpp"
#include <string>
#include <ur_api.h>
#include <vector>

namespace ur_loader {

struct ur_adapter_manifest {
  std::string name;
  std::string library;
  ur_adapter_backend_t backend;
  std::vector<ur_device_type_t> device_types;
};

const std::vector<ur_adapter_manifest> ur_adapter_manifests = {
    {"opencl",
     MAKE_LIBRARY_NAME("ur_adapter_opencl", "0"),
     UR_ADAPTER_BACKEND_OPENCL,
     {// this should be changed to DEVICE_TYPE_ALL in the final thing
      UR_DEVICE_TYPE_CPU, UR_DEVICE_TYPE_GPU, UR_DEVICE_TYPE_FPGA}},
    {"cuda",
     MAKE_LIBRARY_NAME("ur_adapter_cuda", "0"),
     UR_ADAPTER_BACKEND_CUDA,
     {UR_DEVICE_TYPE_GPU}},
    {"hip",
     MAKE_LIBRARY_NAME("ur_adapter_hip", "0"),
     UR_ADAPTER_BACKEND_HIP,
     {UR_DEVICE_TYPE_GPU}},
    {
        "level_zero",
        MAKE_LIBRARY_NAME("ur_adapter_level_zero", "0"),
        UR_ADAPTER_BACKEND_LEVEL_ZERO,
        {UR_DEVICE_TYPE_GPU},
    },
    {
        "native_cpu",
        MAKE_LIBRARY_NAME("ur_adapter_native_cpu", "0"),
        UR_ADAPTER_BACKEND_NATIVE_CPU,
        {UR_DEVICE_TYPE_CPU},
    }};
} // namespace ur_loader
