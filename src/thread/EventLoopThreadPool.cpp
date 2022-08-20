#include "EventLoopThreadPool.h"
#include "EventLoop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int numThreads
    , const std::string& name)
    : _next(0), _numThreads(numThreads),
      _started(false), _name(name),
      _baseLoop(baseLoop)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    assert(!_started);
    _baseLoop->assertInLoopThread();
    _started = true;

    for(int i=0; i<_numThreads; i++){
        EventLoopThread* t = new EventLoopThread(_name + "_thread_" + std::to_string(i));
        _threads.emplace_back(t);
        _loops.emplace_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(_started);
    _baseLoop->assertInLoopThread();

    EventLoop* loop = _baseLoop;
    if(!_loops.empty()){
        loop = _loops[_next++];
        if(static_cast<size_t>(_next) >= _loops.size()){
            _next = 0;
        }
    }
    return loop;
}

