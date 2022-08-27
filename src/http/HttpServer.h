#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <list>

#include "../socket/TcpConnection.h"
#include "../socket/TcpServer.h"
#include "../schedular/TimerQueue.h"

class HttpServer
{
public:
    HttpServer(EventLoop* loop, uint16_t port, int idleSeconds = 10);
    ~HttpServer();

    void init(const std::string& cwd);
    void start();

    void onConnection(const TcpConnection::TcpConnectionPtr& conn);
    void onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buf, TimeStamp time);
    void onTimer();

private:
    typedef std::list<std::weak_ptr<TcpConnection>> WeakConnectionList;
    struct Node{
        TimeStamp lastReciveTime;
        WeakConnectionList::iterator poistion;
        HttpConn http;
    };

    std::string _cwd;
    int _idleSeconds;
    TcpServer _server;
    WeakConnectionList _connections;
};

#endif // HTTPSERVER_H

