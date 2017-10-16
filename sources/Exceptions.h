#pragma once

#include <exception>
#include <string>

#include "StringBuilder.h"

namespace idfc
{
    class IdfcException : public std::exception
    {
    protected:
        std::string description;
        
    public:
        IdfcException(const std::string& description) : description(description) 
        {
        }
        
    public:
        virtual const char* what() const throw() override 
        { 
            return description.c_str(); 
        }
    };

    class AssertionException : public IdfcException
    {
    public:
        AssertionException(const std::string& description) : IdfcException(description)
        {
        }
    };
    
    class JsonCppException : public IdfcException
    {
    public:
        JsonCppException(const std::string& description) : IdfcException(std::string("JsonCppException: ") + description)
        {
        }
    };
    
    class CurlException : public IdfcException
    {
    public:
        CurlException(const std::string& description) : IdfcException(description)
        {
        }
    };
    
    class ApacheException : public IdfcException
    {
    public:
        ApacheException(const std::string& description) : IdfcException(description)
        {
        }
    };

    class IoException : public IdfcException
    {
    public:
        IoException(const std::string& description) : IdfcException(description) { }
    };

    class NetworkException : public IoException
    {
    public:
        NetworkException(const std::string& description) : IoException(description) { }
    };

    class LoadLibraryException : public IdfcException
    {
    public:
        LoadLibraryException(const std::string& description) : IdfcException(description) { }
    };
}
