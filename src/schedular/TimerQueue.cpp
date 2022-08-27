#include "TimerQueue.h"
#include "../thread/EventLoop.h"

int createTimerfd()
{
    int fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
    if(fd <= 0){
        LOG_ERROR << "timerfd_create error";
    }
    return fd;
}

void resetTimerfd(int timerfd, TimeStamp when)
{
    struct itimerspec newValue;
    newValue.it_value.tv_sec = TimeStamp::timeDifference(when, TimeStamp::now()) / 1000;
    if(timerfd_settime(timerfd, 0, &newValue, nullptr) == -1){
        LOG_ERROR << "timerfd_settime error";
    }
}

void readTimerfd(int timerfd)
{
    uint16_t howmany;
    ssize_t n = read(timerfd, &howmany, sizeof howmany);
    if(n == -1){
        LOG_ERROR << "readTimerfd error";
    }
}

TimerQueue::TimerQueue(EventLoop* loop)
    : _loop(loop),
      _timerfd(createTimerfd()),
      _timerfdChannel(Channel(loop, _timerfd))
{
    _timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    _timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue()
{

}

void TimerQueue::addTimer(TimeStamp when, double interval, Timer::TimeCallback cb)
{   
    Timer* timer = new Timer(when, interval, std::move(cb)); 
    _loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    _loop->assertInLoopThread();
    bool earliestChanged = _addTimer(timer);
    if(earliestChanged){
        resetTimerfd(_timerfd, timer->expiration());
    }
}

void TimerQueue::handleRead()
{
    _loop->assertInLoopThread();
    readTimerfd(_timerfd);
    TimeStamp now(TimeStamp::now());
    std::vector<Entry> expired = _getExpired(now);

    for(const Entry& expire : expired){
        expire.second->run();
    }

    _reset(expired, now);
}

bool TimerQueue::_addTimer(Timer* timer)
{
    _loop->assertInLoopThread();
    bool earliestChanged = false;
    TimeStamp when = timer->expiration();
    TimerList::iterator it = _timers.begin();
    if(it == _timers.end() || when < it->first){
        earliestChanged = true;
        auto result = _timers.insert(Entry(when, timer));
        assert(result.second);
    }
    return earliestChanged;
}

std::vector<TimerQueue::Entry> TimerQueue::_getExpired(TimeStamp now)
{
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = _timers.lower_bound(sentry);
    assert(it == _timers.end() || now < it->first);
    std::copy(_timers.begin(), it, back_inserter(expired));
    _timers.erase(_timers.begin(), it);

    return expired;
}

void TimerQueue::_reset(const std::vector<TimerQueue::Entry>& expired, TimeStamp now)
{
    TimeStamp nextExpire;

    for(const Entry& it : expired){
        if(it.second->isRepeat()){
            it.second->restart(now);
            _addTimer(it.second);
        }
        else{
            delete it.second;   // fixme
        }
    }

    if (!_timers.empty())
    {
        nextExpire = _timers.begin()->second->expiration();
    }

    resetTimerfd(_timerfd, nextExpire);
}