#include "TimeStamp.h"

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
}

size_t TimeStamp::timeDifference(TimeStamp high, TimeStamp low)
{
    return std::chrono::duration_cast<MS>(high._expires - low._expires).count();
}