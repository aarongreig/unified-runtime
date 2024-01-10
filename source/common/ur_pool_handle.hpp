#include "umf_helpers.hpp"
#include "ur_pool_manager.hpp"
#include <umf_pools/disjoint_pool_config_parser.hpp>

namespace usm {

struct pool_handle_base {
    std::atomic_uint32_t RefCount = 1;
    ur_context_handle_t Context = nullptr;

    usm::DisjointPoolAllConfigs DisjointPoolConfigs =
        usm::DisjointPoolAllConfigs();

    usm::pool_manager<usm::pool_descriptor> PoolManager;

    pool_handle_base(ur_context_handle_t Context, ur_usm_pool_desc_t *PoolDesc);
    uint32_t incrementReferenceCount() noexcept { return ++RefCount; }
    uint32_t decrementReferenceCount() noexcept { return --RefCount; }
    uint32_t getReferenceCount() const noexcept { return RefCount; }

    bool hasUMFPool(umf_memory_pool_t *umf_pool) {
        return PoolManager.hasPool(umf_pool);
    }

    std::optional<umf_memory_pool_handle_t>
    getOrCreatePool(usm::pool_descriptor &desc);

    virtual std::pair<ur_result_t, umf::pool_unique_handle_t>
    createUMFPoolForDesc(usm::pool_descriptor &, usm::DisjointPoolConfig) {
        return std::pair<ur_result_t, umf::pool_unique_handle_t>{
            UR_RESULT_ERROR_UNSUPPORTED_FEATURE, nullptr};
    }
};
} // namespace usm
