#include "HttpConn.h"
#include "HTTP.h"

HttpConn::HttpConn()
{

}

HttpConn::~HttpConn()
{

}

void HttpConn::parse(Buffer* buff)
{
    if(_request.isDone()){
        _request.init();
    }

    _request.parse(buff);
    _response.setPath(_request.getPath());
    _response.setCodeState(_request.codeStatus());
}

void HttpConn::writeToBuffer(Buffer* buff)
{
    _response.writeToBuffer(buff);
}