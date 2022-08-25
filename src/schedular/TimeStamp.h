#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <time.h>
#include <chrono>
#include <functional>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;

class TimeStamp
{
public:

    TimeStamp(Clock::time_point time);
    ~TimeStamp();

    bool operator<(const TimeStamp& t){
        return _expires < t._expires;
    }

    bool operator==(const TimeStamp& t){
        return _expires == t._expires;
    }

    static TimeStamp now(){
        return TimeStamp(Clock::now());
    }

    void adjust(Clock::time_point t)
    {
        _expires = t;
    }

    TimeStamp& addTime(int ms);
    TimeStamp& addTime(const TimeStamp& t);
    static size_t timeDifference(TimeStamp high, TimeStamp low);

private:
    Clock::time_point _expires;
};

#endif // TIME_H