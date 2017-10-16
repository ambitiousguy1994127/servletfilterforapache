#include "Cache.h"

namespace idfc
{
    Cache::Entry::Entry(time_t expirationTime, const std::vector<char>& payload) : 
        expirationTime(expirationTime), payload(payload)
    {
    }
    
    bool Cache::Entry::isExpired() const
    {
        auto currentTime = static_cast<int>(::time(0));
        return (static_cast<int>(expirationTime) <= currentTime);
    }
    
    Cache::Cache() : cleanerThread(&Cache::cleanerThreadFunction, this)
    {
    }
    
    Cache::~Cache()
    {
        isDestructorCalled.raise();
        cleanerThread.join();
    }
    
    std::shared_ptr<Cache::Entry> Cache::add(const std::string& key, const std::vector<char>& value, int lifeTime)
    {
        auto expired = static_cast<int>(::time(0)) + lifeTime;
        std::shared_ptr<Entry> entry(new Entry(expired, value));
        std::lock_guard<std::mutex> locker(entriesMutex);
        entries[key] = entry;
        return entry;
    }
    
    std::shared_ptr<Cache::Entry> Cache::find(const std::string& key) const
    {
        std::shared_ptr<Cache::Entry> result;
        std::lock_guard<std::mutex> locker(entriesMutex);
        auto i = entries.find(key);
        if (i != entries.cend())
        {
            auto entry = i->second;
            if (!entry->isExpired()) result = entry;
        }
        return result;
    }
    
    void Cache::cleanerThreadFunction()
    {
        static const int kCleanupIntervalMs = 60 * 1000;
        while (!isDestructorCalled.waitFor(kCleanupIntervalMs))
        {
            std::map<std::string, std::shared_ptr<Cache::Entry>>::iterator entryIterator;
            {
                std::lock_guard<std::mutex> locker(entriesMutex);
                if (entries.empty()) continue;
                entryIterator = entries.begin();
            }
            
            while (true)
            {
                auto entry = entryIterator->second;
                if (entry->isExpired())
                {
                    std::lock_guard<std::mutex> locker(entriesMutex);
                    entryIterator = entries.erase(entryIterator);
                }
                else
                {
                    std::lock_guard<std::mutex> locker(entriesMutex);
                    if (++entryIterator == entries.end()) break;
                }
            }
        }
    }
}