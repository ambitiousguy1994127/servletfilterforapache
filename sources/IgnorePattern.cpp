#include "IgnorePattern.h"
#include "StringProcessor.h"

namespace idfc
{
    void IgnorePattern::setIgnoredExtensions(const std::string& values)
    {
        ignoredExtensions = prepareValues(values);
    }
    
    void IgnorePattern::setIgnoredHosts(const std::string& values)
    {
        ignoredHosts = prepareValues(values);
    }
    
    void IgnorePattern::setIgnoredUrls(const std::string& values)
    {
        ignoredHosts = prepareValues(values);
    }
        
    std::set<std::string> IgnorePattern::prepareValues(const std::string& initialString)
    {
        std::set<std::string> result;
        static const std::string delimiter(",");
        auto loweredInitialString = StringProcessor::toLowerCase(initialString);
        auto delimitedValues = StringProcessor::split(loweredInitialString, delimiter);
        for (auto& delimitedValue : delimitedValues)
            result.emplace(std::move(delimitedValue));
        return result;
    }
    
    bool IgnorePattern::isRequestShouldBeRejected(request_rec* request) const
    {
        if (!ignoredExtensions.empty())
        {
            auto uri = request->uri;
            if (uri)
            {
                auto extension = StringProcessor::getExtension(uri, '/');
                if (ignoredExtensions.find(extension) != ignoredExtensions.cend()) return true;
            }
        }
        
        if (!ignoredHosts.empty())
        {
            if (request->hostname)
            {
                std::string host(request->hostname);
                if (ignoredHosts.find(host) != ignoredHosts.cend()) return true;
            }
        }
        
        if (!ignoredUrls.empty())
        {
            if (request->unparsed_uri)
            {
                std::string unparsedUri(request->unparsed_uri);
                if (ignoredUrls.find(unparsedUri) != ignoredUrls.cend()) return true;
            }
        }
        
        return false;
    }
}
