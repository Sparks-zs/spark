#include "HttpServer.h"

HttpServer::HttpServer(EventLoop* loop, uint16_t port)
    : _cwd("./"), _server(TcpServer(loop, port))
{
    _server.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    _server.setReadCallback(std::bind(
        &HttpServer::onRead, this, std::placeholders::_1, std::placeholders::_2));
}

HttpServer::~HttpServer()
{

}

void HttpServer::init(const std::string& cwd)
{
    _cwd = cwd;
}

void HttpServer::start()
{
    _server.start();
}

void HttpServer::onConnection(const TcpConnection::TcpConnectionPtr& conn)
{
    if(conn->connected()){
        conn->setHttpConn(HttpConn(_cwd));
    }
}

void HttpServer::onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buff)
{
    HttpConn* http = conn->getHttpConn();
    if(http->parse(buff)){
        Buffer write_buf;
        http->writeToBuffer(&write_buf);
        conn->send(&write_buf);
    }
}
