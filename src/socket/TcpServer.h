#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "./TcpConnection.h"
#include "./Accpetor.h"
#include "../thread/EventLoop.h"

#include <functional>
#include <vector>
#include <set>

class EventLoopThreadPool;

class TcpServer
{
public:
    TcpServer(EventLoop* loop, uint16_t port, int numThreadPool=5);
    ~TcpServer();

    void start();
    void stop();
    void setThreadPoolSize(int size);

    void newConnection(int clientFd);
    void setConnectionCallback(const TcpConnection::ConnectionCallback& cb){
        _connectionCallback = cb;
    }
    void setReadCallback(const TcpConnection::ReadCallback& cb){
        _readCallback = cb;
    }
    void setWriteCallback(const TcpConnection::WriteCallback& cb){
        _writeCallback = cb;
    }

    void removeConnection(const TcpConnection::TcpConnectionPtr& conn);
    
private:
    EventLoop* _loop;
    EventLoopThreadPool* _ioLoops;
    Accpetor* _accpetor;
    std::set<TcpConnection::TcpConnectionPtr> _connections;

    TcpConnection::ConnectionCallback _connectionCallback;
    TcpConnection::ReadCallback _readCallback;
    TcpConnection::WriteCallback _writeCallback;
    
};
#endif // TCPSERVER_H