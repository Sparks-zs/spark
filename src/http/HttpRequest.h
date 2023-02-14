#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <unordered_map>

#include "Buffer.h"
#include "Log.h"
#include "HTTP.h"

class HttpRequest
{
public:
    // http报文解析状态码
    enum PARSE_STATE {
        REQUEST_LINE = 0,
        REQUEST_HEADERS,
        REQUEST_BODY,
        FINISH,        
    };

    HttpRequest();
    ~HttpRequest();

    void init();
    bool parse(Buffer* buff);

    int codeStatus() { return _code; }
    bool isKeepAlive(){ 
        if(_headers.count("Connection")){
            return _headers.find("Connection")->second == "keep-alive" && _version == "1.1";
        }
        return false;
    }

    std::string getMethod() const { return _method; }
    std::string getPath() const { return _path; }
    std::string getHeader(const std::string& field) const;

private:
    bool _parseRequestLine(const std::string& line);
    bool _parseRequestHeader(const std::string& header);
    bool _parseRequestBody(const std::string& body);

    void _parsePath();
    void _parsePost();

    PARSE_STATE _state;
    HTTP_CODE_STATUS _code;
    std::string _method, _path, _version, _body;
    std::unordered_map<std::string, std::string> _headers;
    std::unordered_map<std::string, std::string> _post;
};


#endif // HTTPREQUEST_H