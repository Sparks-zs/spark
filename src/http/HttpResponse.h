#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>
#include <unordered_map>

#include "../filemanager/filemanager.h"
#include "../log/Log.h"

class Buffer;

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void init();
    
    void setCodeState(int code);
    void setPath(const std::string& path) { _path = path; }
    void writeToBuffer(Buffer* buffer) { buffer->append(_responseContent); }

    void makeResponse();

private:
    void _addStateLine();
    void _addHeader(const std::string& header, const std::string& field);
    void _addBody(const Buffer& buff);

    int _code;
    std::string _path;
    Buffer _responseContent;

    FileManager _fileManager;
};

#endif // HTTPRESPONSE_H