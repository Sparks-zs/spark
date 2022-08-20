#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <condition_variable>
#include "thread.h"

class EventLoop;

class EventLoopThread
{
public:
    EventLoopThread(const std::string& name);
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void _threadFunc();
    std::mutex _mutex;
    std::condition_variable _cond;
    EventLoop* _loop;
    Thread _thread;
};

#endif // EVENTLOOPTHREAD_H