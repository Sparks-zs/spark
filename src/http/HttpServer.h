#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "../socket/TcpConnection.h"
#include "../socket/TcpServer.h"

class HttpServer
{
public:
    HttpServer(EventLoop* loop, uint16_t port);
    ~HttpServer();

    void start();

    void onConnection(const TcpConnection::TcpConnectionPtr& conn);
    void onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buf);

private:
    TcpServer _server;
};

#endif // HTTPSERVER_H

