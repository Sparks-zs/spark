#ifndef EPOLLER_H
#define EPOLLER_H

#include <map>
#include <vector>
#include <assert.h>

struct epoll_event;

class EventLoop;
class Channel;

class Epoller
{
public:
    typedef std::vector<Channel*> ChannelList;
    
    Epoller(EventLoop* loop);
    ~Epoller();

    void poll(ChannelList* activeChannels, int timeoutMs=-1);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void update(int operation, Channel* channel);

private:
    void _fillActiveChannels(int numEvents, ChannelList* channels) const;
    typedef std::vector<struct epoll_event> EventList;
    typedef std::map<int, Channel*> ChannelMap;

    int _epollfd;
    EventLoop* _ownerLoop;
    EventList _events;
    ChannelMap _channels;
};


#endif // EPOLLER_H