#pragma once

#include <sys/stat.h>

namespace idfc
{
    class FileInfo
    {
    private:
        struct stat statBuffer;
        
    public:
        FileInfo(const std::string& fileName);
        
    public:
        int getModificationTime() const;
    };
}
