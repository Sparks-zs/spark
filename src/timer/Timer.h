#ifndef TIMER_H
#define TIMER_H

#include "TimeStamp.h"

class Timer
{
public:
    typedef std::function<void()> TimeCallback;
    Timer(TimeStamp when, int interval, const TimeCallback& cb)
        : _expiration(when),
          _interval(interval),
          _cb(cb)
    {}

    TimeStamp expiration() { return _expiration; }
    bool isRepeat() { return _interval > 0.0; }
    void run() { if(_cb) _cb(); }

    void restart(TimeStamp now);

private:
    TimeStamp _expiration;
    int _interval;
    const TimeCallback _cb;

};

#endif // TIMER_H