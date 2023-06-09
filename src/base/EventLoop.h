#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "CurrentThread.h"
#include "TimerQueue.h"
#include "Log.h"

class Channel;
class Epoller;

class EventLoop
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void wakeup();

    // 检查每个线程是否都只有一个EventLoop
    void assertInLoopThread(){
        if(!isInLoopThread()){
            LOG_WARN << "This eventloop is not in its thread";
        }
    }

    bool isInLoopThread() const{
        return _threadId == CurrentThread::tid();
    }

    static EventLoop* getEventLoopOfCurrentThread();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
    void runInLoop(const Functor& cb);
    void queueInLoop(const Functor& cb);

    void runEvery(int interval, Timer::TimeCallback cb);
    void runAfter(int interval, Timer::TimeCallback cb);
    void runAt(TimeStamp time, Timer::TimeCallback cb);

private:
    typedef std::vector<Channel*>ChannelList;
    void _handleRead();
    void _doPendingFunctors();

    bool _isLooping;
    bool _quit;
    bool _callingPendingFunctors;
    const pid_t _threadId;
    int _wakeupFd;
    
    std::unique_ptr<Channel> _wakeupChannel;
    std::unique_ptr<Epoller> _epoller;
    ChannelList _activeChannels;
    std::mutex _mutex;
    std::vector<Functor> _pendingFunctors;

    std::unique_ptr<TimerQueue> _timerQueue;
};

#endif // EVENTLOOP_H