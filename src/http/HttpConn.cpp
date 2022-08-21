#include "HttpConn.h"
#include "HTTP.h"

HttpConn::HttpConn(const std::string& cwd)
    : _cwd(cwd)
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

    _response.setPath(_cwd + _request.getPath());
    _response.setCodeState(_request.codeStatus());

    return true;
}

void HttpConn::writeToBuffer(Buffer* buff)
{
    _response.makeResponse();
    _response.writeToBuffer(buff);
}