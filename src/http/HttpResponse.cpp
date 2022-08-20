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
        _fileManager.readFile(_path, &buff);
        _addHeader("Content-Type", _fileManager.GetFileType());
        _addHeader("Content-Length", to_string(_fileManager.Size()));
        _addBody(&buff);
        break;
    case BAD_REQUEST:
        _addHeader("Content-Type", "");
        _addHeader("Content-Length", 0);
        break; 
    case FORBIDDEN:
        _addHeader("Content-Type", "");
        _addHeader("Content-Length", 0);
        break;
    case NOT_FOUND:
        _addHeader("Content-Type", "");
        _addHeader("Content-Length", 0);
        break;
    case METHOD_NOT_ALLOWED:
        _addHeader("Content-Type", "");
        _addHeader("Content-Length", 0);
        break;
    default:
        break;
    }

}

void HttpResponse::_addStateLine()
{
    string line = "HTTP/1.1 " + to_string(_code) + " " +  HTTP_CODE_REASON.find(_code)->second;
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

void HttpResponse::setCodeState(int code)
{
    if(HTTP_CODE_REASON.count(code) == 0){
        LOG_WARN << "当前不支持此HTTP状态码: " << code;
        code = 400;
    }
    _code = code;
}