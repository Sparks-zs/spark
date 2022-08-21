#ifndef HTTPCONN_H
#define HTTPCONN_H

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../buffer/Buffer.h"

class HttpConn
{
public:
    HttpConn();
    ~HttpConn();

    bool parse(Buffer* buff);
    void writeToBuffer(Buffer* buff);

private:
    HttpRequest _request;
    HttpResponse _response;
};

#endif // HTTPCONN_H