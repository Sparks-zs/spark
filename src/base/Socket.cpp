#include "Socket.h"
#include <signal.h>

Address::Address(const std::string& ip, uint16_t port)
    : _ip(ip), _port(port)
{
    _addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr);
    _addr.sin_port = htons(port);
}

Address::Address(uint16_t port)
    : _ip("127.0.0.1"), _port(port)
{
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr.sin_port = htons(port);
}

Address::~Address()
{

}

const struct sockaddr* Address::getSockaddr() const
{
    return (const struct sockaddr*)(&_addr);
}

Socket::~Socket()
{
    //close();
}

bool Socket::bind(Address address)
{
    int ret = ::bind(_socket, address.getSockaddr(), 
                    static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if(ret < 0){
        LOG_ERROR << "socket bind error is " << errno;
        return false;
    }
    return true;
}

bool Socket::listen(int maxConn)
{
    int ret = ::listen(_socket, maxConn);
    if(ret < 0){
        close();
        LOG_ERROR << "socket listen error";
        return false;
    }
    return true;
}

int Socket::accept()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int connfd = ::accept(_socket, (struct sockaddr *)&addr, &len);
    if(connfd <= 0){
        LOG_ERROR << "socket accpet error";
        return -1;
    } 
    return connfd;
}

void Socket::connect(const Address& address, int* err){
    int ret = ::connect(_socket, address.getSockaddr(),
                static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if(ret < 0){
        *err = errno;
    }
    else{
        *err = 0;
    }
}

bool Socket::setsockopt(int level, int optname, const void* optval, socklen_t optlen)
{
    int ret = ::setsockopt(_socket, level, optname, optval, optlen);
    if(ret == -1) {
        close();
        LOG_ERROR << "set socket setsockopt error";
        return false;
    }
    return true;
}

void Socket::close()
{
    ::close(_socket);
}

void Socket::setFdNonblock()
{
    assert(fcntl(_socket, F_SETFL, fcntl(_socket, F_GETFD, 0) | O_NONBLOCK) >= 0);
}

void Socket::setKeepAlive(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE,
                 &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setNoSigPipe(bool on){
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
	signal(SIGPIPE, SIG_IGN);
}