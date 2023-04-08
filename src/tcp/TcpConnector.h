#ifndef TCPCONNECTOR_H
#define TCPCONNECTOR_H

#include "Socket.h"
#include "TcpConnection.h"

class TcpConnector{

public:
    typedef std::function<void(Socket sock)> NewConnectionCallback;
    TcpConnector(EventLoop* loop, const Address& address);
    ~TcpConnector();

    void start();
    void restart();
    void stop();

    void setNewConnectionCallback(const NewConnectionCallback& cb){
        _newConnectionCallback = cb;
    } 

private:
    enum States{
        Disconnected,
        Connecting,
        Connected
    };

    void setState(States s){
        _state = s;
    }

    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(Socket sockfd);
    void handleWrite();
    void handleError();
    void retry(Socket sockfd);
    Socket removeAndResetChannel();
    void resetChannel();

    EventLoop* _loop;
    Address _address;
    States _state;
    bool _isConnecting;
    std::unique_ptr<Channel> _channel;
    NewConnectionCallback _newConnectionCallback;

};

#endif // TCPCONNECTOR_H