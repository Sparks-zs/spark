#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>
#include "EventLoopThread.h"

class EventLoop;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* baseLoop, int numThreads=5, const std::string& name="EventLoopThreadPool");
    ~EventLoopThreadPool();

    void start();
    EventLoop* getNextLoop();
    void setNumThread(int size){
        assert(size > 0);
        _numThreads = size;
    }

private:
    int _next;
    int _numThreads;
    bool _started;
    std::string _name;
    EventLoop* _baseLoop;
    std::vector<std::unique_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop*> _loops;
};

#endif // EVENTLOOPTHREADPOOL_H