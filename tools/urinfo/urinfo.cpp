// Copyright (C) 2023 Intel Corporation
// Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
// See LICENSE.TXT
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "utils.hpp"
#include <cstdlib>
#include <vector>

int main(int, char **) {
    UR_CHECK(urInit(0, nullptr));

    uint32_t adapterCount = 0;

    UR_CHECK(urAdapterGet(0, nullptr, &adapterCount));
    std::vector<ur_adapter_handle_t> adapters(adapterCount);
    UR_CHECK(urAdapterGet(adapterCount, adapters.data(), nullptr));

    uint32_t platformCount = 0;
    UR_CHECK(urPlatformGet(adapters.data(), adapterCount, 0, nullptr,
                           &platformCount));

    std::vector<ur_platform_handle_t> platforms(platformCount);
    UR_CHECK(urPlatformGet(adapters.data(), adapterCount, platformCount,
                           platforms.data(), nullptr));

    auto platform_1 = platforms[0];

    ur_platform_backend_t platform_backend;
    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_BACKEND,
                               sizeof(platform_backend), &platform_backend,
                               nullptr));

    size_t nameSize1 = 0;
    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_NAME, 0, nullptr,
                               &nameSize1));
    std::vector<char> name1(nameSize1);

    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_NAME, nameSize1,
                               name1.data(), nullptr));

    size_t nameSize2 = 0;
    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_NAME, 0, nullptr,
                               &nameSize2));
    std::vector<char> name2(nameSize2);

    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_NAME, nameSize2,
                               name2.data(), nullptr));

    uint32_t numDevices = 0;
    ur_device_type_t device_type = UR_DEVICE_TYPE_DEFAULT;
    urDeviceGet(platform_1, device_type, 0, 0, &numDevices);

    std::vector<ur_device_handle_t> devices(numDevices);

    urDeviceGet(platform_1, device_type, 1, devices.data(), nullptr);

    auto device_1 = devices[0];
    size_t version_size = 0;
    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_VERSION, 0, nullptr,
                               &version_size));

    std::vector<char> version_string(version_size);
    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_VERSION,
                               version_size, version_string.data(), nullptr));

    size_t nameSize3 = 0;
    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_NAME, 0, nullptr,
                               &nameSize3));

    std::vector<char> platform_name(nameSize3);
    UR_CHECK(urPlatformGetInfo(platform_1, UR_PLATFORM_INFO_NAME, 48,
                               platform_name.data(), nullptr));

    ur_device_type_t device_type_q;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_TYPE,
                             sizeof(device_type_q), &device_type_q, nullptr));

    ur_device_handle_t parentDevice = nullptr;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_PARENT_DEVICE,
                             sizeof(parentDevice), &parentDevice, nullptr));

    UR_CHECK(urDeviceRetain(device_1));

    size_t extensionsSize = 0;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS, 0, nullptr,
                             &extensionsSize));

    std::vector<char> deviceExtensions(extensionsSize);
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS,
                             extensionsSize, deviceExtensions.data(), nullptr));

    size_t extensionsSize2 = 0;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS, 0, nullptr,
                             &extensionsSize2));

    std::vector<char> extensions2(extensionsSize2);
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS,
                             extensionsSize2, extensions2.data(), nullptr));

    ur_device_type_t deviceType2;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_TYPE, sizeof(deviceType2),
                             &deviceType2, nullptr));

    uint32_t deviceVendorID = 0;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_VENDOR_ID,
                             sizeof(deviceVendorID), &deviceVendorID, nullptr));

    size_t driverVersionsSize = 0;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_DRIVER_VERSION, 0,
                             nullptr, &driverVersionsSize));

    std::vector<char> deviceDriverVersion(driverVersionsSize);

    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_DRIVER_VERSION,
                             driverVersionsSize, deviceDriverVersion.data(),
                             nullptr));

    size_t deviceNameSize = 0;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_NAME, 0, nullptr,
                             &deviceNameSize));

    std::vector<char> deviceName(deviceNameSize);
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_NAME, deviceNameSize,
                             deviceName.data(), nullptr));

    UR_CHECK(urDeviceRelease(device_1));

    ur_device_type_t deviceType3;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_TYPE, sizeof(deviceType3),
                             &deviceType3, nullptr));

    ur_device_handle_t parentDevice2 = nullptr;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_PARENT_DEVICE,
                             sizeof(parentDevice2), &parentDevice2, nullptr));

    UR_CHECK(urDeviceRetain(device_1));

    size_t extensionsSize3 = 0;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS, 0, nullptr,
                             &extensionsSize3));
    std::vector<char> deviceExtensions2(extensionsSize3);

    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS,
                             extensionsSize3, deviceExtensions2.data(),
                             nullptr));

    size_t extensionsSize4 = 0;
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS, 0, nullptr,
                             &extensionsSize4));

    std::vector<char> deviceExtensions3(extensionsSize4);
    UR_CHECK(urDeviceGetInfo(device_1, UR_DEVICE_INFO_EXTENSIONS,
                             extensionsSize4, deviceExtensions3.data(),
                             nullptr));

    UR_CHECK(urDeviceRelease(device_1));

    UR_CHECK(urDeviceRelease(device_1));

    // investigate if this is really true or not
    auto platform_2_mystery = platform_1;

    ur_platform_backend_t platformBackend2;

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_BACKEND,
                               sizeof(platformBackend2), &platformBackend2,
                               nullptr));

    size_t platformNameSize2 = 0;

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_NAME, 0,
                               nullptr, &platformNameSize2));

    std::vector<char> platformName2(platformNameSize2);

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_NAME,
                               platformNameSize2, platformName2.data(),
                               nullptr));

    size_t platformNameSize3 = 0;

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_NAME, 0,
                               nullptr, &platformNameSize3));

    std::vector<char> platformName3(platformNameSize3);
    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_NAME, 33,
                               platformName3.data(), nullptr));

    uint32_t numDevice2 = 0;

    UR_CHECK(urDeviceGet(platform_2_mystery, device_type, 0, 0, &numDevice2));

    ur_device_handle_t device_2 = nullptr;

    UR_CHECK(
        urDeviceGet(platform_2_mystery, device_type, 1, &device_2, nullptr));

    size_t platformVersionSize = 0;

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_VERSION, 0,
                               nullptr, &platformVersionSize));

    std::vector<char> platformVersion(platformVersionSize);

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_VERSION,
                               platformVersionSize, platformVersion.data(),
                               nullptr));

    size_t platformNameSize4 = 0;

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_NAME, 0,
                               nullptr, &platformNameSize4));

    std::vector<char> platformName4(platformNameSize4);

    UR_CHECK(urPlatformGetInfo(platform_2_mystery, UR_PLATFORM_INFO_NAME, 33,
                               platformName4.data(), nullptr));

    ur_device_type_t deviceType4;

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_TYPE, sizeof(deviceType4),
                             &deviceType4, nullptr));

    ur_device_handle_t device2Parent = nullptr;

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_PARENT_DEVICE,
                             sizeof(device2Parent), &device2Parent, nullptr));

    UR_CHECK(urDeviceRetain(device_2));

    size_t deviceExtensionsSize = 0;

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_EXTENSIONS, 0, nullptr,
                             &deviceExtensionsSize));

    std::vector<char> deviceExtensions4(deviceExtensionsSize);

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_EXTENSIONS,
                             deviceExtensionsSize, &deviceExtensions4,
                             nullptr));

    size_t deviceExtenionsSize2 = 0;

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_EXTENSIONS, 0, nullptr,
                             &deviceExtenionsSize2));

    std::vector<char> deviceExtensions5(deviceExtenionsSize2);

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_EXTENSIONS,
                             deviceExtenionsSize2, deviceExtensions5.data(),
                             nullptr));

    ur_device_type_t deviceType5;

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_TYPE, sizeof(deviceType5),
                             &deviceType5, nullptr));

    uint32_t deviceVendorID2;

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_VENDOR_ID, 4,
                             &deviceVendorID2, nullptr));

    size_t deviceDriverVersionsSize = 0;

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_DRIVER_VERSION, 0,
                             nullptr, &deviceDriverVersionsSize));

    std::vector<char> deviceDriverVersion2(deviceDriverVersionsSize);

    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_DRIVER_VERSION,
                             deviceDriverVersionsSize,
                             deviceDriverVersion2.data(), nullptr));

    size_t deviceNameSize2 = 0;

    // kaboom??
    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_NAME, 0, nullptr,
                             &deviceNameSize2));

    UR_CHECK(urDeviceRelease(device_1));
    UR_CHECK(urDeviceRelease(device_2));
    UR_CHECK(urAdapterRelease(adapters[0]));
    UR_CHECK(urTearDown(nullptr));
    return 0;
}
