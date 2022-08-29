#include "HttpServer.h"

HttpServer::HttpServer(EventLoop* loop, uint16_t port, int idleSeconds)
    : _cwd("./"), _idleMilliSeconds(idleSeconds) ,_server(TcpServer(loop, port))
{
    _server.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    _server.setReadCallback(std::bind(
        &HttpServer::onRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    loop->runEvery(1000, std::bind(&HttpServer::onTimer, this));
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
        node.lastReciveTime = Time::now();
        node.http = HttpConn(_cwd);
        conn->setContext(node);
        _connections.push_back(conn);
        node.poistion = --_connections.end();
    }
}

void HttpServer::onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buff, Time time)
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
    Time now = Time::now();
    for (WeakConnectionList::iterator it = _connections.begin();
      it != _connections.end(); it++)
    {
        TcpConnection::TcpConnectionPtr conn = it->lock();
        if(conn){
            Node* node = std::any_cast<Node>(conn->getContext());
            double age = timeDifference(now, node->lastReciveTime);
            if(age > _idleMilliSeconds){
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