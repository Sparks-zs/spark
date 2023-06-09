#include "HttpRequest.h"
#include <regex>
#include <algorithm>
#include <sstream>

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
    _path = "/";
    _version = "";
    _body = "";
    _route = "";
    _frag = "";
    _headers.clear();
    _post.clear();
    _query.clear();
    _params.clear();
}

bool HttpRequest::parse(Buffer* buff)
{
    LOG_DEBUG << "SERVER RECIVE HTTP DATA :\n" << buff->asString();
    
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
            break;        }

        if(lineEnd == buff->beginWriteConst()) buff->retrieveUntil(lineEnd);
        else buff->retrieveUntil(lineEnd + 2);
    }
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
        return false;
    }

    //_body += body;
    _code = OK;
    _state = FINISH;

    if(getHeader("Content-Type") == "application/x-www-form-urlencoded"){
        _parseBody();
    }

    return true;
}

void HttpRequest::_parsePath()
{
    regex patten("(\\/[^\\?#]*)(\\?([^#]*))?(#(.*))?");
    smatch subMatch;
    if(!regex_match(_path, subMatch, patten)){
        LOG_WARN << "path 解析失败";
        return;
    }
    
    _route = subMatch[1];
    _frag = subMatch[5];

    vector<string> ret = _splitString(subMatch[3], '&');
    for(auto& query : ret){
        vector<string> r = _splitString(query, '=');
        if(r.size() == 2){
            string value;
            for(size_t i = 0; i < r[1].size(); i++){
                if(r[1][i] == '%'){
                    unsigned char high = _converHex(r[1][i + 1]);
                    unsigned char low = _converHex(r[1][i + 2]);
                    value += high * 16 + low;
                    i += 2;
                }
                else if(r[1][i] == '+'){
                    value += ' ';
                }
                else{
                    value += r[1][i];
                }
            }
            _query[r[0]] = value;
        }
    }
}

void HttpRequest::_parseBody()
{
    if(_body.empty()){
        return;
    }

    size_t size = _body.size();
    string key, value;
    bool parse_key = true;
    for(size_t i = 0; i < size; i++){
        if(_body[i] == '='){
            parse_key = false;
            continue;
        }
        if(_body[i] == '&'){
            parse_key = true;
            _params[key] = value;
            key.clear();
            value.clear();
            continue;
        }

        if(parse_key){
            key += _body[i];
        }
        else{
            value += _body[i];
        }
    }

    if(!key.empty() && !value.empty()){
        _params[key] = value;
        key.clear();
        value.clear();
    }
}
bool HttpRequest::isKeepAlive() const{ 
    if(_headers.count("Connection")){
        std::string str = _headers.find("Connection")->second;
        std::string temp = str;
        std::transform(str.begin(), str.end(), temp.begin(), std::towlower);
        return (temp.compare("keep-alive") == 0) && (_version == "1.1");
    }
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

string HttpRequest::getParam(const string& key) const
{
    string value;
    if(_params.count(key)){
        value = _params.find(key)->second;
    }
    return value;
}

string HttpRequest::getQuery(const string& key) const
{
    string value;
    if(_query.count(key)){
        value = _query.find(key)->second;
    }
    return value;
}

vector<uint64_t> HttpRequest::getRange() const{
    vector<uint64_t> ret;
    string bytes_range = getHeader("Range");
    size_t bytes_pos = bytes_range.find("bytes=");
    
    if(bytes_pos != string::npos){
        const vector<string> range = _splitString(bytes_range.substr(bytes_pos + 6), '-');
        for(string r : range){
            ret.push_back(stoll(r));
        }
    }
    return ret;
}

unsigned char HttpRequest::_converHex(unsigned char ch){
    if(ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
    else if(ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
    else if( ch >= '0' && ch <= '9') return ch - '0';
    LOG_ERROR << "16进制转换失败";
    return ch;
}

vector<string> HttpRequest::_splitString(const string& str, char delim) const
{
    stringstream ss(str);
    string item;
    vector<string> elems;
    while (getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}