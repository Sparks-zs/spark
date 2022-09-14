#include "Channel.h"
#include "src/thread/EventLoop.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : _loop(loop), _fd(fd), _events(0), _revents(0),
      _index(-1)
{

}

Channel::~Channel()
{
}

void Channel::_update()
{
    _loop->updateChannel(this);
}

void Channel::handleEvent()
{
    if(_revents & (EPOLLRDHUP | EPOLLHUP)){
        if(_closeCallback) _closeCallback();
    }
    if(_revents & EPOLLIN){
        if(_readCallback) _readCallback();
    }
    if(_revents & EPOLLOUT){
        if(_writeCallback) _writeCallback();
    }

}

void Channel::remove()
{
    _loop->removeChannel(this);
}