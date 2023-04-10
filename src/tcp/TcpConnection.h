#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <any>

#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

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
    typedef std::function<void(const TcpConnectionPtr&, Buffer*, TimeStamp)> ReadCallback;
    typedef std::function<void(const TcpConnectionPtr&)> WriteCallback;
    typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

    TcpConnection(EventLoop* loop,
                  int sockfd, 
                  const std::string& name = "TcpConnection");

    ~TcpConnection();

    void start();
    void close();
    void destroy();

    void send(Buffer* buffer);
    void send(const std::string& str);

    int getSocketFd() const { return _socket->fd(); }
    std::string name() const { return _name; }

    bool connected() const { return _state == Connected; }
    void setState(ConnState s) { _state = s; }
    EventLoop* getLoop() const { return _loop; }

    void setContext(std::any context) { _context = context; }
    std::any* getContext() { return &_context; }

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
    void _closeInLoop();

    EventLoop* _loop;
    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;
    ConnState _state;
    std::string _name;

    std::any _context;

    Buffer _readBuffer;
    Buffer _writeBuffer;

    ConnectionCallback _connectionCallback;
    ReadCallback _readCallback;
    WriteCallback _writeCallback;
    CloseCallback _closeCallback;
};

#endif // TCPCONNECTION_H