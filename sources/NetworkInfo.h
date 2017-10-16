#pragma once

#include <memory>
#include <mutex>
#include <string>

#include "PlatformInfo.h"

namespace idfc
{
    class NetworkInfo
    {
    private:
#ifdef IDFC_LINUX
        std::mutex networkApiMutex;
#endif

    public:
        std::string getFQDN();
    };
}
