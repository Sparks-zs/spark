#include "HttpRequest.h"
#include <regex>
#include <algorithm>

using namespace std;

HttpRequest::HttpRequest()
    : _isDone(true)
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::init()
{
    _code = NO_REQUEST;
    _state = REQUEST_LINE;
    _method = " ";
    _path = "";
    _version = "";
    _body = "";
    _headers.clear();
    _post.clear();
}

bool HttpRequest::parse(Buffer* buff)
{
    const char CRLF[] = "\r\n";
    if(buff->readableBytes() <= 0){
        return false;
    }

    while(buff->readableBytes() && _state != FINISH){
        const char* lineEnd = search(buff->peek(), buff->beginWriteConst(), CRLF, CRLF + 2);
        string line(buff->peek(), lineEnd);
        switch (_state)
        {
        case REQUEST_LINE:
            if(_parseRequestLine(line)){
                return false;
            }
            _parsePath();
            break;
        case REQUEST_HEADERS:
            _parseRequestHeader(line);
            if(buff->readableBytes() <= 2){
                _state = FINISH;
            }
            break;
        case REQUEST_BODY:
            _parseRequestBody(line);
            break;
        default:
            break;
        }
        buff->retrieveUntil(lineEnd + 2);
    }
    _code = OK;
    _isDone = true;
    LOG_DEBUG << "HTTP解析完毕: method: " << _method << " url: " << _path << \
                 " version: " << _version << " body: " << _body;
    return true;
}


bool HttpRequest::_parseRequestLine(const string& line)
{
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)) {   
        _method = subMatch[1];
        _path = subMatch[2];
        _version = subMatch[3];
        _state = REQUEST_HEADERS;
        return true;
    }
    _code = BAD_REQUEST;
    return false;
}

string HttpRequest::getHeader(const string& field) const
{
    string header;
    if(_headers.count(field)){
        header = _headers.find(field)->second;
    }
    return header;
}

bool HttpRequest::_parseRequestHeader(const std::string& header)
{
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if(regex_match(header, subMatch, patten)) {
        _headers[subMatch[1]] = subMatch[2];
    }
    else {
        _state = REQUEST_BODY;
    }
    return true;
}

bool HttpRequest::_parseRequestBody(const string& body)
{
    int len = -1;
    if(_headers.count("Content-Length")){
        len = stoi(_headers.find("Content-Lenght")->second);
    }
    if(len <= 0 || body.size() < static_cast<size_t>(len)){
        if(len) LOG_DEBUG << "当前请求体大小为: " << len;
        else LOG_DEBUG << "无请求体";
        return false;
    }

    _body = body;
    _state = FINISH;
    _parsePost();

    return true;
}

void HttpRequest::_parsePath()
{

}

void HttpRequest::_parsePost()
{

}