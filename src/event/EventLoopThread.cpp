#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const std::string& name)
    : _loop(nullptr), 
      _thread(Thread(std::bind(
        &EventLoopThread::_threadFunc, this), name))
{

}

EventLoopThread::~EventLoopThread()
{

}

EventLoop* EventLoopThread::startLoop()
{
    _thread.detach();
    EventLoop* loop;
    {
        std::unique_lock<std::mutex> locker(_mutex);
        while(_loop == nullptr){
            _cond.wait(locker);
        }
        loop = _loop;
    }
    return loop;
}

void EventLoopThread::_threadFunc()
{
    EventLoop loop;

    {
        std::lock_guard<std::mutex> locker(_mutex);
        _loop = &loop;
        _cond.notify_one();
    }

    loop.loop();
    std::lock_guard<std::mutex> locker(_mutex);
    _loop = nullptr;
}