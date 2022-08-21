#ifndef BUFFER_H
#define BUFFER_H

#include <sys/uio.h>  //readv
#include <string>
#include <vector>
#include <assert.h>

class Buffer
{
public:
    Buffer(size_t size=4096): m_readPos(0), 
        m_writePos(0), m_buf(size){}

    ~Buffer() = default;

    void retrieve(size_t len){
        assert(len <= readableBytes());
        m_readPos += len;
    }

    void retrieveAll(){
        m_readPos += readableBytes();       
    }

    void retrieveUntil(const char* end){
        assert(peek() < end);
        retrieve(end - peek());
    }

    void append(const void* data, size_t len){
        assert(data);
        append(static_cast<const char*>(data), len);
    }
    void append(const char* data, size_t len){
        assert(data);
        ensureWriteable(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const std::string& str){
        append(str.c_str(), str.length());
    }

    void append(const Buffer& buffer){
        append(buffer.peek(), buffer.readableBytes());
    }

    const char* peek() const{ 
        return _begin() + m_readPos;
    }
    
    char* beginWrite(){
        return _begin() + m_writePos;
    }

    const char* beginWriteConst() const{
        return _begin() + m_writePos;
    }

    void shrink(){
        m_buf.shrink_to_fit();
    }

    void addReadedBytes(size_t len){
        assert(len <= readableBytes());
        m_readPos += len;
    }

    void ensureWriteable(size_t len){
        if(len > writeableBytes()){
            _makeSpace(len - writeableBytes());
        }
        assert(len <= writeableBytes());
    }

    void hasWritten(size_t len){
        m_writePos += len;
    }

    const size_t writeableBytes() const{
        return m_buf.size() - m_writePos;
    }

    const size_t readableBytes() const{
        return m_writePos - m_readPos;
    }

    size_t prependableBytes() const{
        return m_readPos;
    }

    ssize_t readFd(int sockFd, int* saveErrno);

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    std::string asString()      // use for Debug
    {
        std::string result(peek(), readableBytes());
        return result;
    }

private:
    char* _begin(){
        return m_buf.data();
    }

    const char* _begin() const{
        return m_buf.data();
    }

    void _makeSpace(size_t len){
        if(len > writeableBytes() + prependableBytes()){
            m_buf.resize(m_writePos + len);
        }
        else{
            size_t readable = readableBytes();
            std::copy(_begin() + m_readPos,
                      _begin() + m_writePos,
                      _begin());
            m_readPos = 0;
            m_writePos = m_readPos + readable;
            assert(readable == readableBytes());
        }
    }

    size_t m_readPos;
    size_t m_writePos;
    std::vector<char> m_buf;
};

#endif // BUFFER_H