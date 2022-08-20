#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Socket.h"
#include "./Channel.h"
#include "../buffer/Buffer.h"
#include "../thread/EventLoop.h"

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    enum ConnState{
        Connecting,
        Connected,
        DisConnected
    };

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr&, Buffer*)> ReadCallback;
    typedef std::function<void(const TcpConnectionPtr&)> WriteCallback;
    typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

    TcpConnection(EventLoop* loop, int sockfd, const std::string& name="TcpConnection");
    ~TcpConnection();

    void start();
    void stop();
    void destroy();
    void send(Buffer* buffer);
    void send(const std::string& str);
    void setState(ConnState s) { _state = s; }
    EventLoop* getLoop() { return _loop; }

    // 供channel类回调
    void handelRead();
    void handelWrite();
    void handleError();     // 处理连接出错
    void handleClose();     // 处理连接关闭

    // 供外部Server类回调
    void setConnectionCallback(const ConnectionCallback& cb){
        _connectionCallback = cb;
    }
    void setReadCallback(const ReadCallback& cb){
        _readCallback = cb;
    }
    void setWriteCallback(const WriteCallback& cb){
        _writeCallback = cb;
    }
    void setCloseCallback(const CloseCallback& cb){
        _closeCallback = cb;
    }

    void connectionEstablished();
    
private:
    Socket* _socket;
    EventLoop* _loop;
    Channel _channel;
    ConnState _state;
    std::string _name;
    
    Buffer _readBuffer;
    Buffer _writeBuffer;

    ConnectionCallback _connectionCallback;
    ReadCallback _readCallback;
    WriteCallback _writeCallback;
    CloseCallback _closeCallback;
};

#endif // TCPCONNECTION_H