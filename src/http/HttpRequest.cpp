#include "HttpRequest.h"
#include <regex>
#include <algorithm>

using namespace std;

HttpRequest::HttpRequest()
{
    init();
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
    if(_state == FINISH) init();

    const char CRLF[] = "\r\n";
    while(_state != FINISH){
        if(buff->readableBytes() <= 0) return false;
        const char* lineEnd = search(buff->peek(), buff->beginWriteConst(), CRLF, CRLF + 2);
        if(lineEnd == buff->beginWriteConst() && _state!=REQUEST_BODY){
            return false;
        }
        if(_state == REQUEST_BODY) lineEnd = buff->beginWriteConst();
        string line(buff->peek(), lineEnd);
        LOG_DEBUG << "state: "<< _state << " line: " << line;

        switch (_state)
        {
        case REQUEST_LINE:
            _parseRequestLine(line);
            _parsePath();
            break;
        case REQUEST_HEADERS:
            _parseRequestHeader(line);
            if(buff->readableBytes() <= 2){
                _code = OK;
                _state = FINISH;
            }
            break;
        case REQUEST_BODY:
            _parseRequestBody(line);
            break;
        default:
            break;
        }

        if(lineEnd == buff->beginWriteConst()) buff->retrieveUntil(lineEnd);
        else buff->retrieveUntil(lineEnd + 2);
    }
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
    _state = FINISH;
    return false;
}

bool HttpRequest::_parseRequestHeader(const std::string& header)
{
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if(regex_match(header, subMatch, patten)) {
        _headers[subMatch[1]] = subMatch[2];
    }
    else{
        _state = REQUEST_BODY;
    }
    return true;
}

bool HttpRequest::_parseRequestBody(const string& body)
{
    int len = -1;
    if(!_headers.count("Content-Length")){
        _code = BAD_REQUEST;
        _state = FINISH;
        return false;
    }
    len = stoi(_headers.find("Content-Length")->second);

    _body += body;

    if(len <= 0 || _body.size() < static_cast<size_t>(len)){
        if(len) LOG_DEBUG << "当前请求体大小为: " << len;
        else LOG_DEBUG << "无请求体";
        return false;
    }

    _body = body;
    _code = OK;
    _state = FINISH;

    _parsePost();

    return true;
}

void HttpRequest::_parsePath()
{
    if(_path == "/"){
        _path = "/index.html"; 
    }
}

void HttpRequest::_parsePost()
{

}

string HttpRequest::getHeader(const string& field) const
{
    string header;
    if(_headers.count(field)){
        header = _headers.find(field)->second;
    }
    return header;
}