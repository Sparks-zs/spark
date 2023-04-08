#include "TcpClient.h"

TcpClient::TcpClient(std::string host, uint16_t port)
    : _hostAddress(host, port),
      _loop(new EventLoop()),
      _connector(new TcpConnector(_loop, _hostAddress)),
      _connectionCallback(defaultConnectionCallback),
      _readCallback(defaultReadCallback),
      _writeCallback(defaultWriteCallback){
    
    _connector->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}

TcpClient::~TcpClient(){

}

void TcpClient::connect(){
    _connector->start();
    _loop->loop();
}

void TcpClient::stop(){
    _connector->stop();
}

void TcpClient::newConnection(Socket sock){
    _loop->assertInLoopThread();
    TcpConnection::TcpConnectionPtr conn(
        new TcpConnection(_loop,
                          sock.fd(),
                          "TcpConnection_" + std::to_string(sock.fd())));

    conn->setConnectionCallback(_connectionCallback);
    conn->setReadCallback(_readCallback);
    conn->setWriteCallback(_writeCallback);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));

    _conn = conn;
    
    conn->connectionEstablished();
}

void TcpClient::removeConnection(const TcpConnection::TcpConnectionPtr& conn){
    _loop->assertInLoopThread();
    assert(_loop == conn->getLoop());
    LOG_DEBUG << "连接断开";
    
    _loop->queueInLoop(std::bind(&TcpConnection::destroy, conn));
    _conn.reset();
}