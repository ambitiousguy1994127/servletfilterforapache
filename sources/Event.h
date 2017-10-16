#pragma once

#include <condition_variable>
#include <mutex>

namespace idfc
{
    class Event
    {
    private:
        bool isRaised;
        std::condition_variable conditionVariable;
        std::mutex conditionVariableMutex;

    public:
        Event();
        virtual ~Event();

    public:
        void raise();
        void wait();
        bool waitFor(size_t maximumIntervalInMilliseconds);

    private:
        bool isReallyRaised() const;
    };
}
