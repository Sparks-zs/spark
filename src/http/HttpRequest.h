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

    int codeStatus() const { return _code; }
    bool isKeepAlive(){ 
        if(_headers.count("Connection")){
            return _headers.find("Connection")->second == "keep-alive" && _version == "1.1";
        }
        return false;
    }
    bool finish() const { return _code == OK; }

    std::string getMethod() const { return _method; }
    std::string getPath() const { return _path; }
    std::string getRoute() const { return _route; }
    std::string getFrag() const { return _frag; }
    std::string getParam(const std::string& key) const;
    std::string getHeader(const std::string& field) const;
    std::string getQuery(const std::string& key) const;
    std::string getBody() const { return _body; }


private:
    bool _parseRequestLine(const std::string& line);
    bool _parseRequestHeader(const std::string& header);
    bool _parseRequestBody(const std::string& body);

    void _parsePath();
    void _parseBody();

    std::vector<std::string> _splitString(const std::string& str, char delim);

    PARSE_STATE _state;
    HTTP_CODE_STATUS _code;
    std::string _method, _path, _version, _body, _route, _frag;
    std::unordered_map<std::string, std::string> _headers, _post, _query, _params;

};


#endif // HTTPREQUEST_H