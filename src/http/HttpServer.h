#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <list>

#include "TcpConnection.h"
#include "TcpServer.h"
#include "TimerQueue.h"
#include "HttpConn.h"

class HttpServer
{
public:
    typedef std::function<void(const HttpRequest&, HttpResponse&)> HttpCallback;
    typedef std::unordered_map<std::string, HttpCallback> Handler;

    HttpServer(int16_t port, EventLoop* loop = new EventLoop(), uint idleSeconds = 5000);
    ~HttpServer();

    void start();

    void onConnection(const TcpConnection::TcpConnectionPtr& conn);
    void onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buf, TimeStamp time);
    void onTimer();

    void get(std::string url, HttpCallback cb){
        _getHandler[url] = std::move(cb);
    }
    void post(std::string url, HttpCallback cb){
        _postHandler[url] = std::move(cb);
    }
    void put(std::string url, HttpCallback cb){
        _postHandler[url] = std::move(cb);
    }
    void patch(std::string url, HttpCallback cb){
        _postHandler[url] = std::move(cb);
    }
    void del(std::string url, HttpCallback cb){
        _postHandler[url] = std::move(cb);
    }

private:
    void _onHandle(const HttpRequest& requeset, HttpResponse& response, const std::string& method);
    void _handle(const HttpRequest& requeset, HttpResponse& response, Handler handler);
    typedef std::list<std::weak_ptr<TcpConnection>> WeakConnectionList;
    struct Node{
        TimeStamp lastReciveTime;
        //WeakConnectionList::iterator poistion;
        HttpConn http;
    };

    int _idleMilliSeconds;
    EventLoop* _loop;
    TcpServer _server;
    WeakConnectionList _connections;

    Handler _getHandler;
    Handler _postHandler;
    Handler _putHandler;
    Handler _patchHandler;
    Handler _delHandler;
};

#endif // HTTPSERVER_H

