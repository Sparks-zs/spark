#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "./TcpConnection.h"
#include "./Accpetor.h"
#include "../thread/EventLoop.h"
#include "../schedular/heaptimer.h"

#include <functional>
#include <vector>
#include <set>

class EventLoopThreadPool;

class TcpServer
{
public:
    TcpServer(EventLoop* loop, uint16_t port, int numThreadPool=5, int timeoutMs=10000);
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

    static void defaultConnectionCallback(const TcpConnection::TcpConnectionPtr& conn){

    }
    static void defaultReadCallback(const TcpConnection::TcpConnectionPtr& conn, Buffer* buff){
        
    }
    static void defaultWriteCallback(const TcpConnection::TcpConnectionPtr& conn){
        
    }
    
private:
    EventLoop* _loop;
    EventLoopThreadPool* _ioLoops;
    Accpetor* _accpetor;
    int _timeoutMs;
    HeapTimer::HeapTimerPtr _timer;
    std::set<TcpConnection::TcpConnectionPtr> _connections;

    TcpConnection::ConnectionCallback _connectionCallback;
    TcpConnection::ReadCallback _readCallback;
    TcpConnection::WriteCallback _writeCallback;
    
};
#endif // TCPSERVER_H