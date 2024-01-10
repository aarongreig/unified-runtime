//===--------- usm.hpp - CUDA Adapter -------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
//
// Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM
// Exceptions. See LICENSE.TXT
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "ur_pool_handle.hpp"
#include <umf_helpers.hpp>
#include <umf_pools/disjoint_pool_config_parser.hpp>
#include <ur_pool_manager.hpp>

struct ur_usm_pool_handle_t_ final : public usm::pool_handle_base {
  ur_usm_pool_handle_t_(ur_context_handle_t Context,
                        ur_usm_pool_desc_t *PoolDesc)
      : pool_handle_base(Context, PoolDesc){};

  std::pair<ur_result_t, umf::pool_unique_handle_t>
  createUMFPoolForDesc(usm::pool_descriptor &,
                       usm::DisjointPoolConfig) override;
};

class ClUSMSharedMemoryProvider : public umf::USMSharedMemoryProvider {
  ur_result_t allocateImpl(void **ResultPtr, size_t Size,
                           uint32_t Alignment) override;
};

class ClUSMHostMemoryProvider : public umf::USMHostMemoryProvider {
  ur_result_t allocateImpl(void **ResultPtr, size_t Size,
                           uint32_t Alignment) override;
};

class ClUSMDeviceMemoryProvider : public umf::USMDeviceMemoryProvider {
  ur_result_t allocateImpl(void **ResultPtr, size_t Size,
                           uint32_t Alignment) override;
};

ur_result_t USMDeviceAllocImpl(void **ResultPtr, ur_context_handle_t Context,
                               ur_device_handle_t Device,
                               ur_usm_device_mem_flags_t *Flags, size_t Size,
                               uint32_t Alignment);

ur_result_t USMSharedAllocImpl(void **ResultPtr, ur_context_handle_t Context,
                               ur_device_handle_t Device,
                               const ur_usm_desc_t *USMDesc, size_t Size);

ur_result_t USMHostAllocImpl(void **ResultPtr, ur_context_handle_t Context,
                             ur_usm_host_mem_flags_t *Flags, size_t Size,
                             uint32_t Alignment);
