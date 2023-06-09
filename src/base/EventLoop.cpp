#include <sys/eventfd.h>
#include "EventLoop.h"
#include "epoller.h"
#include "Channel.h"
#include "TimerQueue.h"

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

EventLoop::EventLoop()
    : _isLooping(false), _quit(true),
      _callingPendingFunctors(false),
      _threadId(CurrentThread::tid()),
      _wakeupFd(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
      _wakeupChannel(new Channel(this, _wakeupFd)),
      _epoller(new Epoller(this)),
      _timerQueue(new TimerQueue(this))
{
    if(t_loopInThisThread){
        LOG_WARN << "another Eventloop " << t_loopInThisThread << " exist in this thread " << _threadId;
    }
    else{
        t_loopInThisThread = this;
    }

    _wakeupChannel->setReadCallback(std::bind(&EventLoop::_handleRead, this));
    _wakeupChannel->enableReading();
}

EventLoop::~EventLoop()
{
    assert(_isLooping);
    t_loopInThisThread = nullptr;
    _wakeupChannel->disableAll();
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!_isLooping);
    assertInLoopThread();
    _isLooping = true;
    _quit = false;

    while(!_quit){
        _activeChannels.clear();
        _epoller->poll(&_activeChannels, kPollTimeMs);
        for(ChannelList::iterator iter = _activeChannels.begin();
            iter != _activeChannels.end(); iter++){
            (*iter)->handleEvent();
        }
        _doPendingFunctors();
    }
}

void EventLoop::quit()
{
    _quit = true;
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    _epoller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    _epoller->removeChannel(channel);
}

void EventLoop::runInLoop(const Functor& cb)
{
    if(isInLoopThread()){
        cb();
    }
    else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        std::lock_guard<std::mutex> locker(_mutex);
        _pendingFunctors.push_back(cb);
    }

    if(!isInLoopThread() || _callingPendingFunctors){
        wakeup();
    }
}

void EventLoop::runEvery(int interval, Timer::TimeCallback cb)
{
    TimeStamp time(TimeStamp::now() + interval);
    _timerQueue->addTimer(time, interval, std::move(cb));
}

void EventLoop::runAfter(int interval, Timer::TimeCallback cb)
{
    TimeStamp time(TimeStamp::now() + interval);
    runAt(time, std::move(cb));
}

void EventLoop::runAt(TimeStamp time, Timer::TimeCallback cb)
{
    _timerQueue->addTimer(time, 0.0, std::move(cb));
}


void EventLoop::_doPendingFunctors()
{
    std::vector<Functor>functors;
    _callingPendingFunctors = true;

    {
        std::lock_guard<std::mutex> locker(_mutex);
        functors.swap(_pendingFunctors);
    }

    for(size_t i = 0; i < functors.size(); i++){
        functors[i]();
    }
    _callingPendingFunctors = false;
}

void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = write(_wakeupFd, &one, sizeof one);
}

void EventLoop::_handleRead()
{
  uint64_t one = 1;
  ssize_t n = read(_wakeupFd, &one, sizeof one);
}