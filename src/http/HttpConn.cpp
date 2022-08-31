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
    LOG_DEBUG << "待解析的数据: " << buff->asString();
    if(!_request.parse(buff)){
        return false;
    }
    _response.setCodeState(_request.codeStatus());
    if(_handleCallback) _handleCallback(_request, _response, _request.getMethod());

    return true;
}

void HttpConn::writeToBuffer(Buffer* buff)
{
    _response.makeResponse();
    _response.writeToBuffer(buff);
}