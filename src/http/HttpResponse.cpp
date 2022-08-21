#include "HttpResponse.h"
#include "../buffer/Buffer.h"
#include "HTTP.h"

using namespace std;

HttpResponse::HttpResponse()
    : _code(-1)
{

}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::init()
{
    _responseContent.retrieveAll();
}

void HttpResponse::makeResponse()
{
    init();
    _addStateLine();
    Buffer buff;

    switch(_code){
    case OK:
        _fileManager.readToBuffer(_path, &buff);
        _addHeader("Connection", "keep-alive");
        _addHeader("Content-Type", _fileManager.type());
        _addHeader("Content-Length", to_string(_fileManager.size()));
        break;
    case BAD_REQUEST:
    case FORBIDDEN:
    case NOT_FOUND:
    case METHOD_NOT_ALLOWED:
        buff.append("Return Nothing");
        _addHeader("Content-Type", "text/plain");
        _addHeader("Content-Length", to_string(buff.readableBytes()));
        break;
    default:
        break;
    }

    _addBody(buff);
}

void HttpResponse::setCodeState(int code)
{
    if(HTTP_CODE_REASON.count(code) == 0){
        LOG_WARN << "当前不支持此HTTP状态码: " << code;
        code = BAD_REQUEST;
    }
    _code = code;
}

void HttpResponse::_addStateLine()
{
    string line = "HTTP/1.1 " + to_string(_code) + " " +  HTTP_CODE_REASON.find(_code)->second + "\r\n";
    _responseContent.append(line);
}

void HttpResponse::_addHeader(const std::string& head, const std::string& field)
{
    string header = head + ": " + field + "\r\n";
    _responseContent.append(header);
}

void HttpResponse::_addBody(const Buffer& buff)
{
    _responseContent.append("\r\n");
    _responseContent.append(buff);
}

