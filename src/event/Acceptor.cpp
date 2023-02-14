#include "Acceptor.h"
#include "EventLoop.h"

Accpetor::Accpetor(EventLoop* loop, const Address& address)
    : _socket(Socket(createSocket())), _listening(false),
      _acceptChannel(Channel(loop, _socket.fd()))
{
    _socket.bind(address);
    // // 优雅关闭: 直到所剩数据发送完毕或超时
    // struct linger optLinger = { 0 };
    // optLinger.l_onoff = 1;
    // optLinger.l_linger = 1;
    // int ret;
    // ret = _socket.setsockopt(SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));

    /* 端口复用 */
    int optval = 1;
    _socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    
    _socket.setFdNonblock();
    
    _acceptChannel.setReadCallback(std::bind(&Accpetor::handleRead, this));
}

Accpetor::~Accpetor()
{

}

void Accpetor::listen()
{
    _socket.listen();
    _listening = true;
    _acceptChannel.enableReading();
}

void Accpetor::handleRead()
{
    // 建立连接
    int connfd = _socket.accept();
    LOG_DEBUG << "监听到新用户: "<< connfd;
    if(_newConnectionCallback){
        _newConnectionCallback(connfd);
    }
    else{
        LOG_DEBUG << "socket " << connfd << " is closing";
        ::close(connfd);
    }
}