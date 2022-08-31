#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <list>

#include "../socket/TcpConnection.h"
#include "../socket/TcpServer.h"
#include "../schedular/TimerQueue.h"

class HttpServer
{
public:
    typedef std::function<void(const HttpRequest&, HttpResponse&)> HttpCallback;
    typedef std::unordered_map<std::string, HttpCallback> Handler;

    HttpServer(EventLoop* loop, uint16_t port, int idleSeconds = 5000);
    ~HttpServer();

    void start();

    void onConnection(const TcpConnection::TcpConnectionPtr& conn);
    void onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buf, Time time);
    void onTimer();

    void Get(std::string url, HttpCallback cb){
        _getHandler[url] = std::move(cb);
    }
    void Post(std::string url, HttpCallback cb){
        _postHandler[url] = std::move(cb);
    }

private:
    void _onHandle(const HttpRequest& requeset, HttpResponse& response, const std::string& method);
    void _handle(const HttpRequest& requeset, HttpResponse& response, Handler handler);
    typedef std::list<std::weak_ptr<TcpConnection>> WeakConnectionList;
    struct Node{
        Time lastReciveTime;
        WeakConnectionList::iterator poistion;
        HttpConn http;
    };

    int _idleMilliSeconds;
    TcpServer _server;
    WeakConnectionList _connections;

    Handler _getHandler;
    Handler _postHandler;
};

#endif // HTTPSERVER_H

