#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>
#include <unordered_map>

class Buffer;

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void init();
    void makeResponse(Buffer& buff);

private:
    void _addStateLine(Buffer& buff);
    void _addHeader(Buffer& buff);
    void _addBody(Buffer& buff);

    int _code;

    static const std::unordered_map<int, std::string> CODE_STATUS;      // code对应状态
    static const std::unordered_map<int, std::string> CODE_PATH;        // code对应html路径
};

#endif // HTTPRESPONSE_H