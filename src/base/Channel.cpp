#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
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
    if((_revents & EPOLLHUP)){
        LOG_DEBUG << "channel " << _fd <<" close";
        if(_closeCallback) _closeCallback();
    }
    else if(_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        if(_readCallback) _readCallback();
    }
    else if(_revents & EPOLLOUT){
        if(_writeCallback) _writeCallback();
    }
    else if(_revents & (EPOLLERR)){
        if(_errorCallback) _errorCallback();
    }
}

void Channel::remove()
{
    _loop->removeChannel(this);
}