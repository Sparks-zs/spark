#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "TcpConnector.h"
#include "TcpConnection.h"

class TcpClient{

public:
    TcpClient(std::string host, uint16_t port);
    ~TcpClient();

    void connect();
    void stop();

    void newConnection(Socket sock);
    void setConnectionCallback(const TcpConnection::ConnectionCallback& cb){
        _connectionCallback = cb;
    }
    void setReadCallback(const TcpConnection::ReadCallback& cb){
        _readCallback = cb;
    }
    void setWriteCallback(const TcpConnection::WriteCallback& cb){
        _writeCallback = cb;
    }

    void removeConnection(const TcpConnection::TcpConnectionPtr& conn);
    
    static void defaultConnectionCallback(const TcpConnection::TcpConnectionPtr& conn){
        
    }
    static void defaultReadCallback(const TcpConnection::TcpConnectionPtr& conn, Buffer* buff, TimeStamp time){
        
    }
    static void defaultWriteCallback(const TcpConnection::TcpConnectionPtr& conn){

    }
    
private:
    Address _hostAddress;
    EventLoop* _loop;
    TcpConnector* _connector;
    TcpConnection::TcpConnectionPtr _conn;

    TcpConnection::ConnectionCallback _connectionCallback;
    TcpConnection::ReadCallback _readCallback;
    TcpConnection::WriteCallback _writeCallback;
};

#endif // TCPCLIENT_H