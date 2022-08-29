#include "Timer.h"

void Timer::restart(Time now)
{
    if(isRepeat()){
        _expiration = now + _interval;
    }
}