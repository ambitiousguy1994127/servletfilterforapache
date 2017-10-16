#include "IdfcFiltersPool.h"

namespace idfc
{
    std::shared_ptr<IdfcFilter> IdfcFiltersPool::operator[](const std::string& virtualServerName)
    {
        std::lock_guard<std::mutex> locker(poolMutex);
        auto i = pool.find(virtualServerName);
        if (i == pool.cend())
        {
            std::shared_ptr<IdfcFilter> filter(new IdfcFilter());
            pool[virtualServerName] = filter;
            return filter;
        }
        else
        {
            return i->second;
        }
    }
}
