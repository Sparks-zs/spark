#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <time.h>
#include <chrono>
#include <functional>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef std::chrono::time_point<Clock, MS> MS_TimePoint;

class Time
{
public:

    Time(MS_TimePoint time)
        : _timePoint(time) {}
    Time(){}

    MS_TimePoint time(){
        return _timePoint; 
    }

    void reset(MS_TimePoint t){
        _timePoint = t; 
    }

    static Time now(){
        return Time(std::chrono::time_point_cast<MS>(Clock::now()));
    }

    Time& operator+(int ms){
        _timePoint += MS(ms);
        return *this;
    }

    Time& operator+(Time t){
        _timePoint += MS(_timePoint - t.time());
        return *this;
    }

private:    
    MS_TimePoint _timePoint;
};

inline bool operator<(Time lhs, Time rhs){
    return lhs.time() < rhs.time();
}

inline bool operator==(Time lhs, Time rhs){
    return lhs.time() == rhs.time();
}

inline int64_t timeDifference(Time high, Time low)
{
    return (high.time() - low.time()).count();
}

#endif // TIME_H