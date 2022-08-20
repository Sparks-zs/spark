#ifndef HTTPCONN_H
#define HTTPCONN_H

#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpConn
{
public:
    HttpConn();
    ~HttpConn();

    void parse();

private:
    HttpRequest _request;
    HttpResponse _response;
};


#endif // HTTPCONN_H