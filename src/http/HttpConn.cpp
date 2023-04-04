#include "HttpConn.h"
#include "HTTP.h"

HttpConn::HttpConn()
{
}

HttpConn::~HttpConn()
{

}

bool HttpConn::parse(Buffer* buff)
{
    if(!_request.parse(buff)){
        return false;
    }
    _response.setCodeState(_request.codeStatus());
    _response.setKeepAlive(_request.isKeepAlive());
    if(_handleCallback) _handleCallback(_request, _response, _request.getMethod());

    return true;
}

void HttpConn::writeToBuffer(Buffer* buff)
{
    _response.makeResponse();
    _response.writeToBuffer(buff);
}