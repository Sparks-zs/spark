#include "TcpConnection.h"

TcpConnection::TcpConnection(EventLoop* loop,
                             int sockfd,
                             const std::string& name)
    : _socket(new Socket(sockfd)), _loop(loop), 
     _channel(Channel(loop, sockfd)), _state(Connecting),
     _name(name)
{
    _channel.setReadCallback(std::bind(&TcpConnection::handelRead, this));
    _channel.setWriteCallback(std::bind(&TcpConnection::handelWrite, this));
    _channel.setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    _channel.setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection()
{
    if(_state != DisConnected){
        LOG_DEBUG << _name << "处于连接状态, 但正在析构";
    }
    LOG_DEBUG << _name << "正在析构";
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
    _channel.disableAll();
    _channel.remove();
}

void TcpConnection::send(Buffer* buffer)
{
    _writeBuffer.append(buffer->peek(), buffer->readableBytes());
    if(!_channel.isWriting()){
        _channel.enableWriting();
    }
}

void TcpConnection::send(const std::string& str)
{
    _writeBuffer.append(str);
    if(!_channel.isWriting()){
        _channel.enableWriting();
    }
}

void TcpConnection::handelRead()
{
    int saveErr;
    ssize_t n = _readBuffer.readFd(_channel.fd(), &saveErr);
    LOG_DEBUG << _name << " readed " << n << " bytes";
    if(n > 0){
        _readCallback(shared_from_this(), &_readBuffer, Time::now());
    }
    else if(n == 0){
        handleClose();
    }
    else{
        handleError();
    }
    
}

void TcpConnection::handelWrite()
{
    LOG_DEBUG << "发送的数据: " << _writeBuffer.asString();
    if(_channel.isWriting()){
        ssize_t n = write(_channel.fd(),
                          _writeBuffer.peek(),
                          _writeBuffer.readableBytes());
        if(n > 0){
            _writeBuffer.retrieve(n);
            if(_writeBuffer.readableBytes() == 0){
                _channel.disableWriting();
            }
        }
    }
    _writeCallback(shared_from_this());
}

void TcpConnection::handleError()
{
    LOG_ERROR << "sockfd "<< _channel.fd() << " ";
}

void TcpConnection::handleClose()
{
    LOG_DEBUG << _name << " is closing";
    _closeCallback(shared_from_this());
}

void TcpConnection::connectionEstablished()
{
    _channel.enableReading();
    setState(Connected);
    _connectionCallback(shared_from_this());
}