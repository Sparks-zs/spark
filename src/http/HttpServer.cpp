#include "HttpServer.h"

HttpServer::HttpServer(EventLoop* loop, uint16_t port, int idleSeconds)
    : _cwd("./"), _idleSeconds(idleSeconds) ,_server(TcpServer(loop, port))
{
    _server.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    _server.setReadCallback(std::bind(
        &HttpServer::onRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    loop->runEvery(1.0, std::bind(&HttpServer::onTimer, this));
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
        Node node;
        node.lastReciveTime = TimeStamp::now();
        node.http = HttpConn(_cwd);
        conn->setContext(node);
        _connections.push_back(conn);
        node.poistion = --_connections.end();
    }
}

void HttpServer::onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buff, TimeStamp time)
{
    Node* node = std::any_cast<Node>(conn->getContext());
    if(node->http.parse(buff)){
        Buffer write_buf;
        node->http.writeToBuffer(&write_buf);
        conn->send(&write_buf);
        node->lastReciveTime = time;
    }
}

void HttpServer::onTimer()
{
    TimeStamp now = TimeStamp::now();
    for (WeakConnectionList::iterator it = _connections.begin();
      it != _connections.end();)
    {
        TcpConnection::TcpConnectionPtr conn = it->lock();
        if(conn){
            Node* node = std::any_cast<Node>(conn->getContext());
            double age = TimeStamp::timeDifference(now, node->lastReciveTime) / 1000;
            if(age > _idleSeconds){
                if(conn->connected()){
                    conn->close();
                    LOG_INFO << "关闭连接: " << conn->name();
                }
            }
            else{
                break;
            }
        }
    }
}