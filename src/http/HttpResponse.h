#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "Buffer.h"
#include "Log.h"
#include <unordered_map>
#include "HTTP.h"

class Buffer;

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void init();
    
    void setCodeState(int code);

    void setKeepAlive(bool keep) { _isKeepAlive = keep;}
    
    void setHeader(const std::string& header, const std::string& field);

    void setTransferFilePath(const std::string& path);
    std::string transferFilePath() const { return _filePath; }

    void setContentRange(uint64_t start, uint64_t end, uint64_t size);
    uint64_t getRangeStart() const { return _startPos; }
    uint64_t getRangeEnd() const { return _endPos; }
    uint64_t getTotalSize() const { return _totalSize; }
    bool isRangeContent() const { return _isRange; }
    
    void setContentType(const std::string& type) { _type = type; }

    void setContent(Buffer content){
        _bodyBuffer.retrieveAll();    
        _bodyBuffer.append(content);
    }

    void makeResponse();

    void writeHeaderToBuffer(Buffer* buffer) { buffer->append(_headerBuffer); }

    void writeBodyToBuffer(Buffer* buffer) { buffer->append(_bodyBuffer); }

private:
    void _addStateLine();
    void _addHeader();

    int _code;
    bool _isKeepAlive;
    std::string _type;
    std::string _filePath;

    // use for range
    bool _isRange;
    uint64_t _startPos;
    uint64_t _endPos;
    uint64_t _totalSize;

    std::unordered_map<std::string, std::string> _headers;
    Buffer _headerBuffer;
    Buffer _bodyBuffer;
    


};

#endif // HTTPRESPONSE_H