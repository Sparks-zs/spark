#include "Timer.h"

void Timer::restart(TimeStamp now)
{
    if(isRepeat()){
        _expiration = now + _interval;
    }
}