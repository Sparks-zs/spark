#ifndef ByteArray_H
#define ByteArray_H

#include <sys/uio.h>
#include <string>
#include <string.h>

class ByteArray
{
    class Node{
    public:
        Node();
        Node(size_t s);

        void addReadPos(size_t s);
        void addWritePos(size_t s);

    public:
        char* buf;
        Node* next;
        size_t size;
        size_t read_pos;
        size_t write_pos;
    };

public: 
    ByteArray(size_t s=4096);
    ~ByteArray();

    void read(void* buf, size_t size);
    void read(struct iovec* buf, uint16_t len);
    void write(const void* buf, size_t size);
    void write(struct iovec* buf, uint16_t len);
    
    void clear();
    void clearReaded();
    
    void addReadedBytes(size_t size);
    void addWritedBytes(size_t size);
    size_t writeableBytes();
    size_t readableBytes();

    const char* getReadPos();
    char* getWritePos();

private:
    void _init();
    size_t _nodeSize();
    size_t _capacity();
    void _addReadNode();
    void _addWriteNode();

    size_t m_readPos;
    size_t m_writePos;
    size_t m_nodeSize;

    Node* m_listHeader;
    Node* m_readNode;
    Node* m_writeNode;
};

#endif // ByteArray_H