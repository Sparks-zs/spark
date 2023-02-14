#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "Buffer.h"
#include "Log.h"

class Buffer;

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void init();
    
    void setCodeState(int code);
    void setKeepAlive(bool keep) { _isKeepAlive = keep;}
    void makeResponse();
    void writeToBuffer(Buffer* buffer) { buffer->append(_writeBuffer); }

    void setContent(Buffer content, const std::string& type){
        _content.retrieveAll();     // TODO
        _content.append(content);
        _type = type;
    }

private:
    void _addStateLine();
    void _addHeader(const std::string& header, const std::string& field);
    void _addBody();

    int _code;
    bool _isKeepAlive;
    std::string _type;
    Buffer _content;
    Buffer _writeBuffer;
};

#endif // HTTPRESPONSE_H