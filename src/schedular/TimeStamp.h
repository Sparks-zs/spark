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
    TimeStamp(){}
    ~TimeStamp();

    // bool operator<(TimeStamp t){
    //     return _expires < t._expires;
    // }

    // bool operator==(TimeStamp t){
    //     return _expires == t._expires;
    // }

    static TimeStamp now(){
        return TimeStamp(Clock::now());
    }

    void adjust(Clock::time_point t)
    {
        _expires = t;
    }

    Clock::time_point expiration() { return _expires; }

    TimeStamp& addTime(int ms);
    TimeStamp& addTime(const TimeStamp& t);
    static int timeDifference(TimeStamp high, TimeStamp low);

private:
    Clock::time_point _expires;
};

inline bool operator<(TimeStamp lhs, TimeStamp rhs){
    return lhs.expiration() < rhs.expiration();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs){
    return lhs.expiration() == rhs.expiration();
}


#endif // TIME_H