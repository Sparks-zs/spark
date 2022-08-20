#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <functional>
#include <utility>
#include <memory>
#include <set>
#include <vector>
#include <assert.h>

#include "../socket/Channel.h"

class Timer;
class TimerId;
class Timestamp;
class EventLoop;
class Channel;

class TimerQueue
{
public:
    typedef std::function<void()> TimerCallback;

    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb,
                     Timestamp when,
                     double interval);

    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, std::unique_ptr<Timer>> Entry;
    typedef std::set<Entry> TimerList;

    void handleRead();

    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

    EventLoop* _loop;
    const int _timerfd;
    Channel _timerfdChannel;
    TimerList _timers;
};

#endif // TIMERQUEUE_H