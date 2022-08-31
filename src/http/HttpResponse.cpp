#include "HttpResponse.h"
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
    _writeBuffer.retrieveAll();
}

void HttpResponse::makeResponse()
{
    init();
    _addStateLine();

    switch(_code){
    case OK:
        _addHeader("Connection", "keep-alive");
        _addHeader("Content-Length", to_string(_content.readableBytes()));
        _addHeader("Content-Type", _type);
        break;
    case BAD_REQUEST:
    case FORBIDDEN:
    case NOT_FOUND:
    case METHOD_NOT_ALLOWED:
        _content.append("Return Nothing");
        _addHeader("Content-Length", to_string(_content.readableBytes()));
        _addHeader("Content-Type", "text/plain");
        break;
    default:
        break;
    }

    _addBody();
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
    _writeBuffer.append(line);
}

void HttpResponse::_addHeader(const std::string& head, const std::string& field)
{
    string header = head + ": " + field + "\r\n";
    _writeBuffer.append(header);
}

void HttpResponse::_addBody()
{
    _writeBuffer.append("\r\n");
    _writeBuffer.append(_content);
}

