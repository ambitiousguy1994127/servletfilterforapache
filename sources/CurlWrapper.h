#pragma once

#include <algorithm>
#include <curl/curl.h>
#include <memory>
#include <vector>

#include "Exceptions.h"

namespace idfc
{
    class CurlWrapper
    {
    public:
        typedef std::vector<char> WriteBuffer;
            
    private:
        CURL* curl;
        
    public:
        CurlWrapper();
        CurlWrapper(const CurlWrapper&) = delete;
        CurlWrapper& operator=(const CurlWrapper&) = delete;
        ~CurlWrapper();
        
    public:
        operator CURL*();
        
    public:
        static void verifyResult(CURLcode curlCode);
        static size_t writeCallback(void* newData, size_t elementsQuantity, size_t elementSize, WriteBuffer* outputBuffer);
        std::string getUrlEncodedVersionOf(const std::string& plainText);
    };
}
