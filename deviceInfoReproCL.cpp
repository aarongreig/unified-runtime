#include <iostream>
#include <vector>

#include <CL/cl.h>

#define CHECK_CL_CALL(call)                        \
  if (call != CL_SUCCESS) {                        \
    std::cerr << "encountered CL error: " << call; \
    std::cerr << "from " << #call << std::endl;    \
    return call;                                   \
  }

inline int printPlatformName(cl_platform_id platform) {
  size_t platformNameSize = 0;
  CHECK_CL_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr,
                                  &platformNameSize));

  std::vector<char> platformNameVec(platformNameSize);

  CHECK_CL_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, platformNameSize,
                                  platformNameVec.data(), nullptr));

  std::string platformName(platformNameVec.begin(), platformNameVec.end());

  std::cout << "platform " << platform << " reports name " << platformName
            << std::endl;
  return CL_SUCCESS;
}

int main(int, char**) {
  cl_uint platformCount = 0;
  CHECK_CL_CALL(clGetPlatformIDs(0, nullptr, &platformCount));

  std::vector<cl_platform_id> platforms(platformCount);

  CHECK_CL_CALL(clGetPlatformIDs(platformCount, platforms.data(), nullptr));

  for (const auto& p : platforms) {
    CHECK_CL_CALL(printPlatformName(p));
  }

  if (platformCount < 2) {
    std::cout << "couldn't find enough platforms to repro." << std::endl;
    return 0;
  }

  auto platform = platforms[1];

  auto device_type = CL_DEVICE_TYPE_ALL;

  cl_uint deviceCount = 0;
  CHECK_CL_CALL(
      clGetDeviceIDs(platform, device_type, 0, nullptr, &deviceCount));

  if (deviceCount == 0) {
    std::cout << "platform " << platform << " has no devices." << std::endl;
    return 0;
  }

  std::vector<cl_device_id> devices(deviceCount);

  CHECK_CL_CALL(clGetDeviceIDs(platform, device_type, deviceCount,
                               devices.data(), nullptr));

  auto device = devices[0];

  size_t deviceNameSize = 0;

  CHECK_CL_CALL(
      clGetDeviceInfo(device, CL_DEVICE_NAME, 0, nullptr, &deviceNameSize));

  return 0;
}
