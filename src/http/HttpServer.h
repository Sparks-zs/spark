#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "../socket/TcpConnection.h"
#include "../socket/TcpServer.h"

class HttpServer
{
public:
    HttpServer(EventLoop* loop, uint16_t port);
    ~HttpServer();

    void init(const std::string& cwd);
    void start();

    void onConnection(const TcpConnection::TcpConnectionPtr& conn);
    void onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buf);

private:
    std::string _cwd;
    TcpServer _server;
};

#endif // HTTPSERVER_H

