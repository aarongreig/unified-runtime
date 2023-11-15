// Copyright (C) 2023 Intel Corporation
// Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
// See LICENSE.TXT
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstring>
#include <uur/fixtures.h>

// Test that updating a command-buffer with a single kernel command
// in a way that changes the NDRange configuration.
struct NDRangeUpdateTests : uur::urExpUpdatableCommandBufferTests {
    void SetUp() override {
        program_name = "indexers_usm";
        UUR_RETURN_ON_FATAL_FAILURE(urExpUpdatableCommandBufferTests::SetUp());

        ur_device_usm_access_capability_flags_t shared_usm_flags;
        ASSERT_SUCCESS(
            uur::GetDeviceUSMSingleSharedSupport(device, shared_usm_flags));
        if (!(shared_usm_flags & UR_DEVICE_USM_ACCESS_CAPABILITY_FLAG_ACCESS)) {
            GTEST_SKIP() << "Shared USM is not supported.";
        }

        // Allocate a USM pointer for use as kernel output at index 0
        ASSERT_SUCCESS(urUSMSharedAlloc(context, device, nullptr, nullptr,
                                        allocation_size, &shared_ptr));
        ASSERT_NE(shared_ptr, nullptr);
        std::memset(shared_ptr, 0, allocation_size);

        ASSERT_SUCCESS(urKernelSetArgPointer(kernel, 0, nullptr, &shared_ptr));

        // Add a 3 dimension kernel command to command-buffer and close
        // command-buffer
        ASSERT_SUCCESS(urCommandBufferAppendKernelLaunchExp(
            updatable_cmd_buf_handle, kernel, n_dimensions,
            global_offset.data(), global_size.data(), local_size.data(), 0,
            nullptr, nullptr, &command_handle));
        ASSERT_NE(command_handle, nullptr);

        ASSERT_SUCCESS(urCommandBufferFinalizeExp(updatable_cmd_buf_handle));
    }

    // For each work-item the kernel prints the global id and local id in each
    // of the 3 dimensions to an offset in the output based on global linear
    // id.
    void Validate(std::array<size_t, 3> global_size,
                  std::array<size_t, 3> local_size,
                  std::array<size_t, 3> global_offset) {
        // DPC++ swaps the X & Z dimension for 3 Dimensional kernels
        // between those set by user and SPIR-V builtins.
        // See `ReverseRangeDimensionsForKernel()` in commands.cpp

        std::swap(global_size[0], global_size[2]);
        std::swap(local_size[0], local_size[2]);
        std::swap(global_offset[0], global_offset[2]);

        // Verify global ID and local ID of each work item
        for (size_t x = 0; x < global_size[0]; x++) {
            for (size_t y = 0; y < global_size[1]; y++) {
                for (size_t z = 0; z < global_size[2]; z++) {
                    const size_t global_linear_id =
                        z + (y * global_size[2]) +
                        (x * global_size[1] * global_size[0]);
                    int *wi_ptr = (int *)shared_ptr +
                                  (elements_per_id * global_linear_id);

                    const int global_id_x = wi_ptr[0];
                    const int global_id_y = wi_ptr[1];
                    const int global_id_z = wi_ptr[2];

#if 0
                    std::cout << "("<< x <<"," << y << "," << z << ") --> ";
                    std::cout << global_linear_id << " = ";
                    std::cout << "("<< global_id_x <<"," << global_id_y << ","
                        << global_id_z << ")\n";
#endif

                    EXPECT_EQ(global_id_x, x + global_offset[0]);
                    EXPECT_EQ(global_id_y, y + global_offset[1]);
                    EXPECT_EQ(global_id_z, z + global_offset[2]);

                    const int local_id_x = wi_ptr[3];
                    const int local_id_y = wi_ptr[4];
                    const int local_id_z = wi_ptr[5];

                    EXPECT_EQ(local_id_x, x % local_size[0]);
                    EXPECT_EQ(local_id_y, y % local_size[1]);
                    EXPECT_EQ(local_id_z, z % local_size[2]);
                }
            }
        }
    }

    void TearDown() override {
        if (shared_ptr) {
            EXPECT_SUCCESS(urUSMFree(context, shared_ptr));
        }

        UUR_RETURN_ON_FATAL_FAILURE(
            urExpUpdatableCommandBufferTests::TearDown());
    }

    static constexpr size_t elements_per_id = 6;
    static constexpr size_t n_dimensions = 3;
    static constexpr std::array<size_t, 3> global_size = {8, 8, 8};
    static constexpr std::array<size_t, 3> local_size = {1, 2, 2};
    static constexpr std::array<size_t, 3> global_offset = {0, 4, 4};
    static constexpr size_t allocation_size = sizeof(int) * elements_per_id *
                                              global_size[0] * global_size[1] *
                                              global_size[2];
    void *shared_ptr = nullptr;
    ur_exp_command_buffer_command_handle_t command_handle = nullptr;
};

UUR_INSTANTIATE_DEVICE_TEST_SUITE_P(NDRangeUpdateTests);

// Keep the kernel work dimensions as 3, and update local size and global
// offset.
TEST_P(NDRangeUpdateTests, Update3D) {
    // Run command-buffer prior to update an verify output
    ASSERT_SUCCESS(urCommandBufferEnqueueExp(updatable_cmd_buf_handle, queue, 0,
                                             nullptr, nullptr));
    ASSERT_SUCCESS(urQueueFinish(queue));
    Validate(global_size, local_size, global_offset);

    // Set local size and global offset to update to
    std::array<size_t, 3> new_local_size = {4, 2, 2};
    std::array<size_t, 3> new_global_offset = {3, 2, 1};
    ur_exp_command_buffer_update_kernel_launch_desc_t update_desc = {
        UR_STRUCTURE_TYPE_EXP_COMMAND_BUFFER_UPDATE_KERNEL_LAUNCH_DESC, // stype
        nullptr,                                                        // pNext
        0,                        // numMemobjArgs
        0,                        // numPointerArgs
        0,                        // numValueArgs
        0,                        // numExecInfos
        3,                        // workDim
        nullptr,                  // pArgMemobjList
        nullptr,                  // pArgPointerList
        nullptr,                  // pArgValueList
        nullptr,                  // pArgExecInfoList
        new_global_offset.data(), // pGlobalWorkOffset
        nullptr,                  // pGlobalWorkSize
        new_local_size.data(),    // pLocalWorkSize
    };

    // Update kernel and enqueue command-buffer again
    ASSERT_SUCCESS(
        urCommandBufferUpdateKernelLaunchExp(command_handle, &update_desc));
    ASSERT_SUCCESS(urCommandBufferEnqueueExp(updatable_cmd_buf_handle, queue, 0,
                                             nullptr, nullptr));
    ASSERT_SUCCESS(urQueueFinish(queue));

    // Verify that update occurred correctly
    Validate(global_size, new_local_size, new_global_offset);
}

// Update the kernel work dimensions to 2, and update global size, local size,
// and global offset to new values.
TEST_P(NDRangeUpdateTests, Update2D) {
    // Run command-buffer prior to update an verify output
    ASSERT_SUCCESS(urCommandBufferEnqueueExp(updatable_cmd_buf_handle, queue, 0,
                                             nullptr, nullptr));
    ASSERT_SUCCESS(urQueueFinish(queue));
    ASSERT_SUCCESS(urCommandBufferEnqueueExp(updatable_cmd_buf_handle, queue, 0,
                                             nullptr, nullptr));
    ASSERT_SUCCESS(urQueueFinish(queue));
    Validate(global_size, local_size, global_offset);

    // Set ND-Range configuration to update to
    std::array<size_t, 3> new_global_size = {6, 6, 1};
    std::array<size_t, 3> new_local_size = {3, 3, 1};
    std::array<size_t, 3> new_global_offset = {3, 3, 0};

    // Set dimensions as 2
    ur_exp_command_buffer_update_kernel_launch_desc_t update_desc = {
        UR_STRUCTURE_TYPE_EXP_COMMAND_BUFFER_UPDATE_KERNEL_LAUNCH_DESC, // stype
        nullptr,                                                        // pNext
        0,                        // numMemobjArgs
        0,                        // numPointerArgs
        0,                        // numValueArgs
        0,                        // numExecInfos
        2,                        // workDim
        nullptr,                  // pArgMemobjList
        nullptr,                  // pArgPointerList
        nullptr,                  // pArgValueList
        nullptr,                  // pArgExecInfoList
        new_global_offset.data(), // pGlobalWorkOffset
        new_global_size.data(),   // pGlobalWorkSize
        new_local_size.data(),    // pLocalWorkSize
    };

    // Reset output to remove old values which will no longer have a
    // work-item to overwrite them
    std::memset(shared_ptr, 0, allocation_size);

    // Update kernel and enqueue command-buffer again
    ASSERT_SUCCESS(
        urCommandBufferUpdateKernelLaunchExp(command_handle, &update_desc));
    ASSERT_SUCCESS(urCommandBufferEnqueueExp(updatable_cmd_buf_handle, queue, 0,
                                             nullptr, nullptr));
    ASSERT_SUCCESS(urQueueFinish(queue));

    // Verify that update occurred correctly
    Validate(new_global_size, new_local_size, new_global_offset);
}

// Update the kernel work dimensions to 1, and check that previously
// set global size, local size, and global offset update accordingly.
TEST_P(NDRangeUpdateTests, Update1D) {
    // Run command-buffer prior to update an verify output
    ASSERT_SUCCESS(urCommandBufferEnqueueExp(updatable_cmd_buf_handle, queue, 0,
                                             nullptr, nullptr));
    ASSERT_SUCCESS(urQueueFinish(queue));
    Validate(global_size, local_size, global_offset);

    // Set dimensions to 1
    ur_exp_command_buffer_update_kernel_launch_desc_t update_desc = {
        UR_STRUCTURE_TYPE_EXP_COMMAND_BUFFER_UPDATE_KERNEL_LAUNCH_DESC, // stype
        nullptr,                                                        // pNext
        0,       // numMemobjArgs
        0,       // numPointerArgs
        0,       // numValueArgs
        0,       // numExecInfos
        1,       // workDim
        nullptr, // pArgMemobjList
        nullptr, // pArgPointerList
        nullptr, // pArgValueList
        nullptr, // pArgExecInfoList
        nullptr, // pGlobalWorkOffset
        nullptr, // pGlobalWorkSize
        nullptr, // pLocalWorkSize
    };

    // Reset output to remove old values which will no longer have a
    // work-item to overwrite them
    std::memset(shared_ptr, 0, allocation_size);

    // Update kernel and enqueue command-buffer again
    ASSERT_SUCCESS(
        urCommandBufferUpdateKernelLaunchExp(command_handle, &update_desc));
    ASSERT_SUCCESS(urCommandBufferEnqueueExp(updatable_cmd_buf_handle, queue, 0,
                                             nullptr, nullptr));
    ASSERT_SUCCESS(urQueueFinish(queue));

    // Verify that update occurred correctly
    std::array<size_t, 3> new_global_size = {global_size[0], 1, 1};
    std::array<size_t, 3> new_local_size = {local_size[0], 1, 1};
    std::array<size_t, 3> new_global_offset = {global_offset[0], 0, 0};
    Validate(new_global_size, new_local_size, new_global_offset);
}
