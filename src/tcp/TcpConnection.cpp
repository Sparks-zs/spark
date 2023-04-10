#include "TcpConnection.h"

TcpConnection::TcpConnection(EventLoop* loop,
                             int sockfd,
                             const std::string& name)
    : _loop(loop), _socket(new Socket(sockfd)),  
     _channel(new Channel(loop, sockfd)), _state(Connecting),
     _name(name)
{
    _channel->setReadCallback(std::bind(&TcpConnection::handelRead, this));
    _channel->setWriteCallback(std::bind(&TcpConnection::handelWrite, this));
    _channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    _channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    _socket->setKeepAlive(true);
    _socket->setNoSigPipe(true);
}

TcpConnection::~TcpConnection()
{
    assert(_state == DisConnected);
}

void TcpConnection::start()
{

}

void TcpConnection::close()
{
    _loop->runInLoop(std::bind(&TcpConnection::_closeInLoop, this));
}

void TcpConnection::_closeInLoop()
{
    _loop->assertInLoopThread();
    handleClose();
}

void TcpConnection::destroy()
{
    setState(DisConnected);
    _channel->disableAll();
    _channel->remove();
}

void TcpConnection::send(Buffer* buffer)
{
    _writeBuffer.append(buffer->peek(), buffer->readableBytes());
    if(!_channel->isWriting()){
        _channel->enableWriting();
    }
}

void TcpConnection::send(const std::string& str)
{
    _writeBuffer.append(str);
    if(!_channel->isWriting()){
        _channel->enableWriting();
    }
}

void TcpConnection::handelRead()
{
    int saveErr;
    ssize_t n = _readBuffer.readFd(_channel->fd(), &saveErr);
    if(n > 0){
        _readCallback(shared_from_this(), &_readBuffer, TimeStamp::now());
    }
    else if(n == 0){
        handleClose();
    }
    else{
        LOG_ERROR << "client fd: " << _channel->fd() << "read error code: " << saveErr;
        handleError();
    }
}

void TcpConnection::handelWrite()
{
    if(_channel->isWriting()){
        LOG_DEBUG << "WRITE DATA TO " << _channel->fd();
        ssize_t n = write(_channel->fd(),
                          _writeBuffer.peek(),
                          _writeBuffer.readableBytes());
        if(n > 0){
            _writeBuffer.retrieve(n);
            if(_writeBuffer.readableBytes() == 0){
                _channel->disableWriting();
            }
        }
        else LOG_DEBUG << "write return n: " << n;
    }
    LOG_DEBUG << "WRITE FINISHED";
    _writeCallback(shared_from_this());
}

void TcpConnection::handleError()
{
}

void TcpConnection::handleClose()
{
    _closeCallback(shared_from_this());
}

void TcpConnection::connectionEstablished()
{
    _channel->enableReading();
    setState(Connected);
    _connectionCallback(shared_from_this());
}