#include "HttpServer.h"

HttpServer::HttpServer(EventLoop* loop, uint16_t port, int idleSeconds)
    : _idleMilliSeconds(idleSeconds) ,_server(TcpServer(loop, port))
{
    _server.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    _server.setReadCallback(std::bind(
        &HttpServer::onRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    loop->runEvery(idleSeconds, std::bind(&HttpServer::onTimer, this));
}

HttpServer::~HttpServer()
{

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
        node.http = HttpConn();
        node.http.setHandleCallback(std::bind(&HttpServer::_onHandle, this,
           std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));    // 设置回调，处理用户任务
        conn->setContext(node);
        _connections.push_back(conn);
        node.poistion = --_connections.end();
    }
}

void HttpServer::onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buff, Time time)
{
    Node* node = std::any_cast<Node>(conn->getContext());
    node->lastReciveTime = time;
    if(node->http.parse(buff)){
        Buffer write_buf;
        node->http.writeToBuffer(&write_buf);
        conn->send(&write_buf);
    }
}

void HttpServer::onTimer()      // 每隔idleSeconds检测一次连接是否超时
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
                }
            }
            else{
                break;
            }
        }
    }
}

void HttpServer::_onHandle(const HttpRequest& request, HttpResponse& response, const std::string& method)
{
    if(method == "GET"){
        _handle(request, response, _getHandler);
    }
    else if(method == "POST"){
        _handle(request, response, _postHandler);
    }
 
}

void HttpServer::_handle(const HttpRequest& request, HttpResponse& response, Handler handler)
{
    if(handler.count(request.getPath())){
        handler.find(request.getPath())->second(request, response);
    }
    else{
        response.setCodeState(NOT_FOUND);
    }
}