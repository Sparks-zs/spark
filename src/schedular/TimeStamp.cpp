#include "TimeStamp.h"
#include <assert.h>

TimeStamp::TimeStamp(Clock::time_point t)
    : _expires(t)
{

}

TimeStamp::~TimeStamp()
{

}

TimeStamp& TimeStamp::addTime(int ms)
{
    _expires += MS(ms);
    return *this;
}

TimeStamp& TimeStamp::addTime(const TimeStamp& t)
{
    if(t._expires > _expires){
        _expires += std::chrono::duration_cast<MS>(t._expires - _expires);
    }
    return *this;
}

int TimeStamp::timeDifference(TimeStamp high, TimeStamp low)
{
    assert(low < high);
    return std::chrono::duration_cast<MS>(high._expires - low._expires).count();
}