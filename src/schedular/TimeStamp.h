#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <time.h>
#include <chrono>
#include <functional>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef std::chrono::time_point<Clock, MS> MS_TimePoint;

class TimeStamp
{
public:

    TimeStamp(MS_TimePoint time)
        : _timePoint(time) {}
    TimeStamp(){}

    MS_TimePoint time(){
        return _timePoint; 
    }

    void reset(MS_TimePoint t){
        _timePoint = t; 
    }

    static TimeStamp now(){
        return TimeStamp(std::chrono::time_point_cast<MS>(Clock::now()));
    }

    TimeStamp& operator+(int ms){
        _timePoint += MS(ms);
        return *this;
    }

    TimeStamp& operator+(TimeStamp t){
        _timePoint += MS(_timePoint - t.time());
        return *this;
    }

private:    
    MS_TimePoint _timePoint;
};

inline bool operator<(TimeStamp lhs, TimeStamp rhs){
    return lhs.time() < rhs.time();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs){
    return lhs.time() == rhs.time();
}

inline int64_t timeDifference(TimeStamp high, TimeStamp low)
{
    return (high.time() - low.time()).count();
}

#endif // TIME_H