#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <unordered_map>
#include <string>
#include "../buffer/Buffer.h"
#include "../log/Log.h"

class HttpRequest
{
public:
    // http报文解析状态码
    enum PARSE_STATE {
        REQUEST_LINE,
        REQUEST_HEADERS,
        REQUEST_BODY,
        FINISH,        
    };

    // http请求状态码
    enum HTTP_CODE {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    HttpRequest();
    ~HttpRequest();

    void init();
    bool parse(Buffer& buff);

private:
    bool _parseRequestLine(const std::string& line);
    bool _parseRequestHeader(const std::string& header);
    bool _parseRequestBody(const std::string& body);

    void _parsePath();
    void _parsePost();

    PARSE_STATE _state;
    std::string _method, _path, _version, _body;
    std::unordered_map<std::string, std::string> _header;
    std::unordered_map<std::string, std::string> _post;
    
};


#endif // HTTPREQUEST_H