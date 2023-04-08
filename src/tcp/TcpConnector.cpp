#include "TcpConnector.h"

TcpConnector::TcpConnector(EventLoop* loop, const Address& address)
    : _loop(loop),
      _address(address),
      _state(Disconnected),
      _isConnecting(false){

}

TcpConnector::~TcpConnector(){
    assert(!_channel);
}

void TcpConnector::start(){
    _isConnecting = true;
    _loop->runInLoop(std::bind(&TcpConnector::startInLoop, this));
}

void TcpConnector::restart(){
    _loop->assertInLoopThread();
    setState(Disconnected);
    _isConnecting = true;
    startInLoop();
}

void TcpConnector::stop(){
    _isConnecting = false;
    _loop->queueInLoop(std::bind(&TcpConnector::stopInLoop, this));
}

void TcpConnector::startInLoop(){
    _loop->assertInLoopThread();
    assert(_state == Disconnected);
    if(_isConnecting){
        connect();
    }
    else{
        LOG_WARN << "do not connect";
    }
}
void TcpConnector::stopInLoop(){
    LOG_DEBUG << "stop";
    _loop->assertInLoopThread();
    if(_state == Connected){
        setState(Disconnected);
        Socket sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void TcpConnector::connect(){
    LOG_DEBUG << "start connect";
    int err;
    Socket sockfd = Socket(createSocket());
    sockfd.connect(_address, &err);
    
    switch(err){
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
        break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
        break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_ERROR << "connect error in Connector::startInLoop " << err;
            sockfd.close();
        break;

        default:
            LOG_ERROR << "Unexpected error in Connector::startInLoop " << err;
            sockfd.close();
        break;
    }

}

void TcpConnector::connecting(Socket sockfd){
    setState(Connecting);
    assert(!_channel);
    _channel.reset(new Channel(_loop, sockfd.fd()));
    _channel->setWriteCallback(std::bind(&TcpConnector::handleWrite, this));
    _channel->setErrorCallback(std::bind(&TcpConnector::handleError, this));
    _channel->enableWriting();
}

void TcpConnector::handleWrite(){
    LOG_DEBUG << "connector handle write";
    if(_state == Connecting){
        setState(Connected);
        Socket sock = removeAndResetChannel();
        if(_isConnecting){
            _newConnectionCallback(sock);
        }
        else{
            sock.close();
        }
    }
}

void TcpConnector::handleError(){
    LOG_DEBUG << "connector handle error";
    if(_state == Connecting){
        Socket sock = removeAndResetChannel();
        retry(sock);
    }
}

void TcpConnector::retry(Socket sockfd){
    sockfd.close();
    setState(Disconnected);
    if(_isConnecting){
        LOG_DEBUG << "retry connecting to IP: " << _address.getIp() << "port: " << _address.getPort();
        _loop->runAfter(3000, std::bind(&TcpConnector::startInLoop, this));
    }   
    else{
        LOG_DEBUG << "do not connect";
    }
}

Socket TcpConnector::removeAndResetChannel(){
    _channel->disableAll();
    _channel->remove();
    Socket sockfd = Socket(_channel->fd());
    _loop->queueInLoop(std::bind(&TcpConnector::resetChannel, this));
    return sockfd;
}

void TcpConnector::resetChannel(){
    _channel.reset();
}

