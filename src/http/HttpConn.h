#ifndef HTTPCONN_H
#define HTTPCONN_H

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../buffer/Buffer.h"

class HttpConn
{
public:
    HttpConn(const std::string& cwd = "./");
    ~HttpConn();

    bool parse(Buffer* buff);
    void writeToBuffer(Buffer* buff);

private:
    std::string _cwd;   // 当前的工作目录

    HttpRequest _request;
    HttpResponse _response;

};

#endif // HTTPCONN_H