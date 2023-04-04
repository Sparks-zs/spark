#include "HttpResponse.h"
#include "HTTP.h"


using namespace std;

HttpResponse::HttpResponse()
    : _code(-1), _isKeepAlive(false),
      _type("text/plain")
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
        if(_isKeepAlive) {
            addHeader("Connection", "keep-alive");
            addHeader("keep-alive", "max=6, timeout=120");
        } else{
            addHeader("Connection", "close");
        }
        break;
    case BAD_REQUEST:
    case FORBIDDEN:
    case NOT_FOUND:
    case METHOD_NOT_ALLOWED:
        break;
    default:
        break;
    }
    
    addHeader("Content-Length", to_string(_content.readableBytes()));
    addHeader("Content-Type", _type);
    
    _addHeader();
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

void HttpResponse::addHeader(const std::string& header, const std::string& field)
{
    _headers[header] = field;
}

void HttpResponse::_addStateLine()
{
    string line = "HTTP/1.1 " + to_string(_code) + " " +  HTTP_CODE_REASON.find(_code)->second + "\r\n";
    _writeBuffer.append(line);
}

void HttpResponse::_addHeader(){
    int size = _headers.size();
    for(auto& header : _headers){
        _writeBuffer.append(header.first + ": " + header.second + "\r\n");
    }
}

void HttpResponse::_addBody()
{
    _writeBuffer.append("\r\n");
    _writeBuffer.append(_content);
}

