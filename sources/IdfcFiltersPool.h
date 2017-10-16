#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "IdfcFilter.h"

namespace idfc
{
    class IdfcFiltersPool
    {
    private:
        std::map<std::string, std::shared_ptr<IdfcFilter>> pool;
        std::mutex poolMutex;
        
    public:
        std::shared_ptr<IdfcFilter> operator[](const std::string& virtualServerName);
    };
}
