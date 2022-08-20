#include "epoller.h"
#include "../socket/Channel.h"

#include <stdio.h>
#include <sys/epoll.h>

namespace
{
const int kNew = -1;
const int kAdd = 1;
const int kDel = 2;
}

Epoller::Epoller(EventLoop* loop)
    : _epollfd(epoll_create(5)), 
      _ownerLoop(loop),
      _events(16)
{

}

Epoller::~Epoller()
{

}

int Epoller::poll(ChannelList* activeChannels, int timeoutMs)
{
    int numEvents = epoll_wait(_epollfd,
                               _events.data(),
                               static_cast<int>(_events.size()),
                               timeoutMs);
    if(numEvents > 0){
        _fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t>(numEvents) == _events.size()){
            _events.resize(_events.size() * 2);
        }
    }
}

void Epoller::updateChannel(Channel* channel)
{
    int fd = channel->fd();
    int index = channel->index();
    if(index == kNew || index == kDel){
        channel->setIndex(kAdd);
        update(EPOLL_CTL_ADD, channel);
    }
    else{
        if(channel->isNoneEvent()){
            channel->setIndex(kDel);
            update(EPOLL_CTL_DEL, channel);
        }
        else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::removeChannel(Channel* channel)
{
    int index = channel->index();
    if(index == kAdd){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kDel);
}

void Epoller::update(int operation, Channel* channel)
{
    struct epoll_event ev = {0};
    ev.data.ptr = channel;
    ev.events = channel->events();
    
    if(epoll_ctl(_epollfd, operation, channel->fd(), &ev) < 0){
        printf("epoll update fail\n");
    }
}

void Epoller::_fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= _events.size());
    for(int i=0; i<numEvents; i++){
        Channel *channel = static_cast<Channel*>(_events[i].data.ptr);
        channel->set_revents(_events[i].events);
        activeChannels->push_back(channel);
    }
}