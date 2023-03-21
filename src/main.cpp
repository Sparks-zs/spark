#include "HttpServer.h"

int main()
{   
    LogStream::Instance()->init(0, "./log/");
    HttpServer server(8888);
    server.start();

    return 0;
}