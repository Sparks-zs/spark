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
    if(_request.isDone()){
        _request.init();
    }

    if(!_request.parse(buff)){
        return false;
    }

    _response.setPath(_request.getPath());
    _response.setCodeState(_request.codeStatus());

    return true;
}

void HttpConn::writeToBuffer(Buffer* buff)
{
    _response.makeResponse();
    _response.writeToBuffer(buff);
}