#include "TimerQueue.h"


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = _timers.lower_bound(sentry);
    assert(it == _timers.end() || now < it->first);
    std::copy(_timers.begin(), it, back_inserter(expired));
    _timers.erase(_timers.begin(), it);

    return expired;
}