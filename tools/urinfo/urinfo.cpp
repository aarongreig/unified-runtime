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
    ur_device_type_t device_type = UR_DEVICE_TYPE_DEFAULT;

    // maybe query what the platforms are here
    auto platform_2_mystery = platforms[1];

    uint32_t numDevice2 = 0;

    UR_CHECK(urDeviceGet(platform_2_mystery, device_type, 0, 0, &numDevice2));

    if (numDevice2 == 0) {
        printf("no devices reported for platform %p!\n", platform_2_mystery);
    }
    std::vector<ur_device_handle_t> devices(numDevice2);

    UR_CHECK(urDeviceGet(platform_2_mystery, device_type, 1, devices.data(),
                         nullptr));

    auto device_2 = devices[0];

    size_t deviceNameSize2 = 0;

    // kaboom??
    UR_CHECK(urDeviceGetInfo(device_2, UR_DEVICE_INFO_NAME, 0, nullptr,
                             &deviceNameSize2));

    return 0;
}
