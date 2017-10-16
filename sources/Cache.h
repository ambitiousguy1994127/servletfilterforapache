#pragma once

#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Event.h"

namespace idfc
{
    class Cache
    {
    public:
        struct Entry
        {
            time_t expirationTime;
            std::vector<char> payload;
            
            Entry(time_t expirationTime, const std::vector<char>& payload);
            bool isExpired() const;
        };
        
    private:
        Event isDestructorCalled;
        std::map<std::string, std::shared_ptr<Cache::Entry>> entries;
        mutable std::mutex entriesMutex;
        std::thread cleanerThread;
        
    public:
        Cache();
        ~Cache();
        
    public:
        std::shared_ptr<Cache::Entry> add(const std::string& key, const std::vector<char>& value, int lifeTime);
        std::shared_ptr<Cache::Entry> find(const std::string& key) const;
        
    private:
        void cleanerThreadFunction();
    };
}