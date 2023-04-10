#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h> // socket api
#include <sys/types.h>  // socket类型 tcp or udp
#include <arpa/inet.h>  // ip地址转换
#include <netinet/in.h> // 字节序转换
#include <unistd.h>     // 连接关闭
#include <fcntl.h>
#include <memory>

#include "SocketsOps.h"
#include "Buffer.h"
#include "Log.h"

class Address
{
public:
    Address(const std::string& ip, uint16_t port);
    Address(uint16_t port);
    ~Address();

    std::string getIp() { return _ip; }
    uint16_t getPort() { return _port; }
    const struct sockaddr* getSockaddr() const;

private:
    std::string _ip;
    uint16_t _port;
    struct sockaddr_in _addr;
};

class Socket : public std::enable_shared_from_this<Socket>
{
public:
    explicit Socket(int socketFd) : _socket(socketFd) {}
    ~Socket();

    bool bind(Address address);
    bool listen(int maxConn=5);
    int accept();
    void connect(const Address& address, int* err);
    void close();

    int fd() { return _socket; }
    bool setsockopt(int level, int optname, const void* optval, socklen_t optlen);
    void setFdNonblock();
    void setKeepAlive(bool on);
    void setNoSigPipe(bool on);
    
private:
    int _socket;
};


#endif // SOCKET_H

