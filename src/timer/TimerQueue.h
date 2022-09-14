#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <set>
#include <memory>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>

#include "Timer.h"
#include "src/log/Log.h"
#include "src/tcp/Channel.h"

class EventLoop;

class TimerQueue
{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    void addTimer(TimeStamp when, int interval, Timer::TimeCallback cb);
    void addTimerInLoop(Timer* timer);
    void handleRead();

private:
    typedef std::pair<TimeStamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    bool _addTimer(Timer* timer);
    std::vector<Entry> _getExpired(TimeStamp now);
    void _reset(const std::vector<Entry>& expired, TimeStamp now);

    EventLoop* _loop;
    const int _timerfd;
    Channel _timerfdChannel;
    TimerList _timers;
};

#endif // TIMERQUEUE_H