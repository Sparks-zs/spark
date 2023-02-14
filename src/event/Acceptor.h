#ifndef ACCPETOR_H
#define ACCPETOR_H

#include "Socket.h"
#include "Channel.h"

class EventLoop;

class Accpetor
{
public:
    typedef std::function<void (int clientFd)> NewConnectionCallback;

    Accpetor(EventLoop* loop, const Address& addr);
    ~Accpetor();

    void listen();
    bool isListening() { return _listening == true; }

    void handleRead();
    void setNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb;}
private:
    Socket _socket;
    bool _listening;
    Channel _acceptChannel;

    NewConnectionCallback _newConnectionCallback;
};

#endif // ACCPETOR_H