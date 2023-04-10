#ifndef HTTPCONN_H
#define HTTPCONN_H

#include <functional>

#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpConn
{
public:
    typedef std::function<void(const HttpRequest&, HttpResponse&, const std::string&)> HandleCallback;

    HttpConn();
    ~HttpConn();

    bool parse(Buffer* buff);
    void writeToBuffer(Buffer* buff);
    
    HttpRequest getRequest() const { return _request; }
    HttpResponse getResponse() const { return _response; }

    void setHandleCallback(HandleCallback cb){
        _handleCallback = std::move(cb);
    }

private:
    HttpRequest _request;
    HttpResponse _response;

    HandleCallback _handleCallback;

};

#endif // HTTPCONN_H