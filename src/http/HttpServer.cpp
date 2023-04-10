#include "HttpServer.h"
#include "sfile.h"

const uint64_t MAX_TRANSFER_SIZE = 1024 * 1024 * 10;

HttpServer::HttpServer(int16_t port, uint numThread, uint idleSeconds, EventLoop* loop)
    : _idleMilliSeconds(idleSeconds) ,
      _loop(loop),
      _server(TcpServer(loop, port, numThread))
{
    _server.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    _server.setReadCallback(std::bind(
        &HttpServer::onRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _server.setWriteCallback(std::bind(
        &HttpServer::onWriteCallback, this, std::placeholders::_1));

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
    Buffer write_buf;
    Node* node = std::any_cast<Node>(conn->getContext());
    node->lastReciveTime = time;
    if(node->http.parse(buff)){
        node->http.writeToBuffer(&write_buf);
        LOG_DEBUG << conn->name() << "send header\n" << write_buf.asString();

        size_t writted_size = write_buf.readableBytes();
        std::string filePath = node->http.getResponse().transferFilePath();
        
        if(!filePath.empty()){
            node->file_path = filePath;
        }
        
        if(writted_size < MAX_TRANSFER_SIZE && !filePath.empty()){
            SFile file(filePath);
            uint64_t writeable_size = MAX_TRANSFER_SIZE - writted_size;
            if(node->http.getResponse().isRangeContent()){    // 如果带有range头部信息，则更改读取位置和大小
                uint64_t start = node->http.getResponse().getRangeStart();
                uint64_t end = node->http.getResponse().getRangeEnd();
                node->start_pos = start;
                node->end_pos = end;
                if(end - start + 1 <= writeable_size){
                    writeable_size = end - start + 1;
                }

                Buffer readFileBuffer;
                file.read(&readFileBuffer, node->start_pos, writeable_size);
                node->start_pos += readFileBuffer.readableBytes();
                write_buf.append(readFileBuffer);
            }
            else if(writeable_size > file.size()){   // 文件过大, 分块传输
                node->start_pos = 0;
                node->end_pos = file.size();
                
                Buffer readFileBuffer;
                file.read(&readFileBuffer, node->start_pos, writeable_size);
                node->start_pos += readFileBuffer.readableBytes();
                write_buf.append(readFileBuffer);
            }
            else {     // 文件可以直接传输
                file.read(&write_buf);
            }
        }

        conn->send(&write_buf);
        node->lastReciveTime = TimeStamp::now() + _idleMilliSeconds;
    }
}

void HttpServer::onWriteCallback(const TcpConnection::TcpConnectionPtr& conn){
    Node* node = std::any_cast<Node>(conn->getContext());
    SFile file(node->file_path);
    if(node->start_pos >= node->end_pos){
        return;
    }

    Buffer readFileBuffer;
    file.read(&readFileBuffer, node->start_pos, node->start_pos + 
        std::min(MAX_TRANSFER_SIZE, node->end_pos - node->start_pos + 1) + 1);
    if(readFileBuffer.readableBytes() == 0){
        node->start_pos = 0;
        node->end_pos = 0;
        return;
    }
    node->start_pos += readFileBuffer.readableBytes();
    conn->send(&readFileBuffer);
    node->lastReciveTime = TimeStamp::now() + _idleMilliSeconds;
}

void HttpServer::onTimer()      // 每隔idleMilliSeconds检测一次连接是否超时
{
    
    for (WeakConnectionList::iterator it = _connections.begin();
      it != _connections.end(); it++)
    {
        TcpConnection::TcpConnectionPtr conn = it->lock();
        TimeStamp now = TimeStamp::now();
        if(conn){
            Node* node = std::any_cast<Node>(conn->getContext());
            double age = timeDifference(now, node->lastReciveTime);
            if(age > _idleMilliSeconds){
                if(node->http.getRequest().isKeepAlive()){
                    node->lastReciveTime = now + _idleMilliSeconds;
                    continue;
                }
                else if(conn->connected()){
                    LOG_DEBUG << conn->name() << "超时断开连接";
                    conn->close();
                    it = _connections.erase(it);
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

void HttpServer::_readFileToBuffer(const std::string& filePath, Buffer* buffer, uint64_t from, uint64_t to){
    SFile file(filePath);
    if(!file.exists() || file.isDirectory()){
        return;
    }

    if(to - from > MAX_TRANSFER_SIZE){

    }


    std::string data;
    file.read(data);

}