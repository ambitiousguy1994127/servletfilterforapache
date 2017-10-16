#include "Event.h"

namespace idfc
{
    Event::Event() : isRaised(false)
    {
    }

    Event::~Event()
    {
    }

    void Event::raise()
    {
        isRaised = true;
        conditionVariable.notify_one();
    }

    void Event::wait()
    {
        std::unique_lock<std::mutex> conditionLocker(conditionVariableMutex);
        conditionVariable.wait(conditionLocker, std::bind(&Event::isReallyRaised, this));
    }

    bool Event::waitFor(size_t maximumIntervalInMilliseconds)
    {
        std::unique_lock<std::mutex> conditionLocker(conditionVariableMutex);
        auto waitTime = std::chrono::milliseconds(maximumIntervalInMilliseconds);
        return conditionVariable.wait_for(conditionLocker, waitTime, std::bind(&Event::isReallyRaised, this));
    }

    bool Event::isReallyRaised() const
    {
        return isRaised;
    }
}
