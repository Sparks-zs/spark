#include "TcpServer.h"
#include "EventLoopThreadPool.h"

TcpServer::TcpServer(EventLoop* loop, uint16_t port, int numThreadPool)
    : _loop(loop),
      _ioLoops(new EventLoopThreadPool(loop, numThreadPool, "IoPool")),
      _accpetor(new Accpetor(_loop, port)),
      _connectionCallback(defaultConnectionCallback),
      _readCallback(defaultReadCallback),
      _writeCallback(defaultWriteCallback)
{
    _accpetor->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    for(auto conn : _connections){
        conn->destroy();
    }
}

void TcpServer::start()
{   
    assert(!_accpetor->isListening());
    _ioLoops->start();
    _loop->runInLoop(std::bind(&Accpetor::listen, _accpetor));
    
}

void TcpServer::stop()
{
    
}

void TcpServer::setThreadPoolSize(int size)
{
    assert(size > 0);
    _ioLoops->setNumThread(size);
}

void TcpServer::newConnection(int clientFd)
{
    EventLoop* ioLoop = _ioLoops->getNextLoop();
    TcpConnection::TcpConnectionPtr conn(
        new TcpConnection(ioLoop,
                          clientFd,
                          "TcpConnection_" + std::to_string(clientFd)));

    _connections.insert(conn);
    conn->setConnectionCallback(_connectionCallback);
    conn->setReadCallback(_readCallback);
    conn->setWriteCallback(_writeCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnection::TcpConnectionPtr& conn)
{
    _loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnection::TcpConnectionPtr& conn)
{
    _loop->assertInLoopThread();
    _connections.erase(conn);
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->runInLoop(std::bind(&TcpConnection::destroy, conn));

}