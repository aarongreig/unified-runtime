#include "ur_pool_handle.hpp"

namespace usm {
pool_handle_base::pool_handle_base(ur_context_handle_t Context,
                                   ur_usm_pool_desc_t *PoolDesc)
    : Context(Context) {
    const void *pNext = PoolDesc->pNext;
    while (pNext != nullptr) {
        const ur_base_desc_t *BaseDesc =
            static_cast<const ur_base_desc_t *>(pNext);
        switch (BaseDesc->stype) {
        case UR_STRUCTURE_TYPE_USM_POOL_LIMITS_DESC: {
            const ur_usm_pool_limits_desc_t *Limits =
                reinterpret_cast<const ur_usm_pool_limits_desc_t *>(BaseDesc);
            for (auto &config : DisjointPoolConfigs.Configs) {
                config.MaxPoolableSize = Limits->maxPoolableSize;
                config.SlabMinSize = Limits->minDriverAllocSize;
            }
            break;
        }
        default: {
            throw umf::UsmAllocationException(UR_RESULT_ERROR_INVALID_ARGUMENT);
        }
        }
        pNext = BaseDesc->pNext;
    }

    ur_result_t Ret;
    std::tie(Ret, PoolManager) =
        usm::pool_manager<usm::pool_descriptor>::create();
    if (Ret) {
        throw umf::UsmAllocationException(Ret);
    }

    auto UrUSMPool = reinterpret_cast<ur_usm_pool_handle_t>(this);

    // We set up our pool handle with default pools for each of the three
    // allocation types
    std::vector<usm::pool_descriptor> Descs;
    std::tie(Ret, Descs) =
        usm::pool_descriptor::createDefaults(UrUSMPool, Context);

    for (auto &Desc : Descs) {
        umf::pool_unique_handle_t Pool = nullptr;
        auto PoolType = usm::urTypeToDisjointPoolType(Desc.type);
        std::tie(Ret, Pool) = createUMFPoolForDesc(
            Desc, this->DisjointPoolConfigs.Configs[PoolType]);
        if (Ret) {
            throw umf::UsmAllocationException(Ret);
        }

        PoolManager.addPool(Desc, Pool);
    }
    // Context->addPool(this);
}

std::optional<umf_memory_pool_handle_t>
pool_handle_base::getOrCreatePool(usm::pool_descriptor &desc) {
    auto foundPool = PoolManager.getPool(desc);
    if (foundPool.has_value()) {
        return foundPool.value();
    }

    umf::pool_unique_handle_t newPool;
    ur_result_t Ret = UR_RESULT_SUCCESS;
    auto PoolType = usm::urTypeToDisjointPoolType(desc.type);
    std::tie(Ret, newPool) =
        createUMFPoolForDesc(desc, this->DisjointPoolConfigs.Configs[PoolType]);
    if (Ret) {
        throw umf::UsmAllocationException(Ret);
    }

    PoolManager.addPool(desc, newPool);
    // doing addPool above std::moves newPool
    return PoolManager.getPool(desc);
}

} // namespace usm
