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

    HttpServer(int16_t port, uint numThread = 1, uint idleMilliSeconds = 5000, EventLoop* loop = new EventLoop());
    ~HttpServer();

    void start();
    void setWriteCallback(const TcpConnection::WriteCallback& cb){
        _writeCallback = std::move(cb);
    }
    
    void onConnection(const TcpConnection::TcpConnectionPtr& conn);
    void onRead(const TcpConnection::TcpConnectionPtr& conn, Buffer* buf, TimeStamp time);
    void onWriteCallback(const TcpConnection::TcpConnectionPtr& conn);
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
    typedef std::list<std::weak_ptr<TcpConnection>> WeakConnectionList;

    struct Node{
        TimeStamp lastReciveTime;
        //WeakConnectionList::iterator poistion;
        HttpConn http;

        std::string file_path;
        uint64_t start_pos;
        uint64_t end_pos;  

        Node() : file_path(""), start_pos(0), end_pos(0) {}
    };

    void _onHandle(const HttpRequest& requeset, HttpResponse& response, const std::string& method);
    void _handle(const HttpRequest& requeset, HttpResponse& response, Handler handler);

    void _readFileToBuffer(const std::string& filePath, Buffer* buffer, uint64_t from, uint64_t to);


    int _idleMilliSeconds;
    EventLoop* _loop;
    TcpServer _server;
    WeakConnectionList _connections;
    TcpConnection::WriteCallback _writeCallback;

    Handler _getHandler;
    Handler _postHandler;
    Handler _putHandler;
    Handler _patchHandler;
    Handler _delHandler;
};

#endif // HTTPSERVER_H

