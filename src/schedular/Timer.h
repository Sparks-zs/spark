#ifndef TIMER_H
#define TIMER_H

#include <functional>

#include "TimeStamp.h"

class Timer
{
public:
    typedef std::function<void()> TimeCallback;
    Timer(Time when, int interval, const TimeCallback& cb)
        : _expiration(when),
          _interval(interval),
          _cb(cb)
    {}

    Time expiration() { return _expiration; }
    bool isRepeat() { return _interval > 0.0; }
    void run() { if(_cb) _cb(); }

    void restart(Time now);

private:
    Time _expiration;
    int _interval;
    const TimeCallback _cb;

};

#endif // TIMER_H