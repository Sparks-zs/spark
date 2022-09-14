#include "src/tcp/TcpServer.h"

int main()
{
    EventLoop loop;
    TcpServer* server = new TcpServer(&loop, 8888);
    server->start();

    loop.loop();
    
    return 0;
}