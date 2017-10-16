#pragma once

#include <string>

namespace idfc
{
    class File
    {
    public:
        static std::string readAll(const std::string& fileName);
    };
}
