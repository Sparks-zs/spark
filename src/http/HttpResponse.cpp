#include "HttpResponse.h"

using namespace std;

HttpResponse::HttpResponse(){
    init();
}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::init()
{
    _code = -1;
    _isKeepAlive = false;
    _filePath = "";
    _isRange = false;
    _startPos = 0;
    _endPos = 0;
    _totalSize = 0;
    _headerBuffer.retrieveAll();
    _bodyBuffer.retrieveAll();
}

void HttpResponse::makeResponse()
{
    switch(_code){
    case OK:
    case NO_CONTENT:
    case PARTIAL_CONTENT:
        if(_isKeepAlive) {
            setHeader("Connection", "Keep-Alive");
            setHeader("Keep-Alive", "max=6, timeout=120");
        } else{
            setHeader("Connection", "close");
        }
        break;
    case BAD_REQUEST:
    case FORBIDDEN:
    case NOT_FOUND:
    case METHOD_NOT_ALLOWED:
        setHeader("Connection", "close");
        break;
    default:
        setHeader("Connection", "close");
        break;
    }
    
    uint64_t content_len = _bodyBuffer.readableBytes();
    if(_endPos > 0) content_len += _endPos - _startPos + 1;
    setHeader("Content-Length", std::to_string(content_len));
    setHeader("Content-Type", _type);

    _addStateLine();
    _addHeader();
    _headerBuffer.append("\r\n");
}

void HttpResponse::setCodeState(int code)
{
    if(HTTP_CODE_REASON.count(code) == 0){
        LOG_WARN << "当前不支持此HTTP状态码: " << code;
        code = BAD_REQUEST;
    }
    _code = code;
}

void HttpResponse::setHeader(const std::string& header, const std::string& field)
{
    _headers[header] = field;
}

void HttpResponse::setTransferFilePath(const std::string& path){
    _filePath = path;
}

void HttpResponse::setContentRange(uint64_t start, uint64_t end, uint64_t size){
    _isRange = true;
    _startPos = start;
    _endPos = end;
    _totalSize = size;

    setHeader("Accept-Ranges", "bytes");
    setHeader("Content-Range", "bytes " + std::to_string(start) + 
                                '-' + std::to_string(end) + 
                                '/' + std::to_string(size));
    
}

void HttpResponse::_addStateLine()
{
    string line = "HTTP/1.1 " + to_string(_code) + " " +  HTTP_CODE_REASON.find(_code)->second + "\r\n";
    _headerBuffer.append(line);
}

void HttpResponse::_addHeader(){
    for(auto& header : _headers){
        _headerBuffer.append(header.first + ": " + header.second + "\r\n");
    }
}
