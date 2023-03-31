#include "HttpServer.h"

HttpServer::HttpServer(int16_t port, EventLoop* loop, uint idleSeconds)
    : _idleMilliSeconds(idleSeconds) ,
      _loop(loop),
      _server(TcpServer(loop, port))
{
    _server.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    _server.setReadCallback(std::bind(
        &HttpServer::onRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _loop->runEvery(idleSeconds, std::bind(&HttpServer::onTimer, this));
}

HttpServer::~HttpServer()
{

}

void HttpServer::start()
{
    _server.start();
    _loop->loop();
}

void HttpServer::onConnection(const TcpConnection::TcpConnectionPtr& conn)
{
    if(conn->connected()){
        Node node;
        node.lastReciveTime = TimeStamp::now();
        node.http = HttpConn();
        node.http.setHandleCallback(std::bind(&HttpServer::_onHandle, this,
           std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));    // 设置回调，处理用户任务
        conn->setContext(node);
        _connections.push_back(conn);
        //node.poistion = --_connections.end();
    }
}

void HttpServer::onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buff, TimeStamp time)
{
    Node* node = std::any_cast<Node>(conn->getContext());
    node->lastReciveTime = time;
    if(node->http.parse(buff)){
        Buffer write_buf;
        node->http.writeToBuffer(&write_buf);
        conn->send(&write_buf);
    }
}

void HttpServer::onTimer()      // 每隔idleMilliSeconds检测一次连接是否超时
{
    TimeStamp now = TimeStamp::now();
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
    if(!request.finish()) return;
    if(method == "GET"){
        _handle(request, response, _getHandler);
    }
    else if(method == "POST"){
        _handle(request, response, _postHandler);
    }
    else if(method == "PUT"){
        _handle(request, response, _putHandler);
    }
    else if(method == "PATCH"){
        _handle(request, response, _patchHandler);
    }
    else if(method == "DELETE"){
        _handle(request, response, _delHandler);
    }
}

void HttpServer::_handle(const HttpRequest& request, HttpResponse& response, Handler handler)
{
    if(handler.count(request.getRoute())){
        handler.find(request.getRoute())->second(request, response);
    }
    else{
        response.setCodeState(NOT_FOUND);
    }
}