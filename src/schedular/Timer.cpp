#include "Timer.h"

void Timer::restart(TimeStamp now)
{
    if(isRepeat()){
        _expiration.addTime(now.addTime(_interval));
    }
}