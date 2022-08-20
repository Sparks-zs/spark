#include "bytearray.h"

ByteArray::Node::Node()
    : buf(nullptr), next(nullptr)
    , size(0), read_pos(0), write_pos(0)
{

}

ByteArray::Node::Node(size_t s)
    : buf(new char[s]), next(nullptr)
    , size(s), read_pos(0), write_pos(0)
{
    
}

void ByteArray::Node::addReadPos(size_t s)
{
    if(s <= 0 || s > size)
        return;
    read_pos += s;
}

void ByteArray::Node::addWritePos(size_t s)
{
    if(s <= 0 || s > size)
        return;
    write_pos += s;
}

ByteArray::ByteArray(size_t s)
    : m_readPos(0), m_writePos(0)
    , m_nodeSize(s)
    , m_listHeader(new Node())
    , m_readNode(nullptr)
    , m_writeNode(nullptr)
{
    _init();
}

ByteArray::~ByteArray()
{
    clear();
}

void ByteArray::read(void* buf, size_t size)
{
    if(size <= 0)
        return;
    size_t readed_size = 0;
    while(size > 0){
        size_t capacity = m_readNode->write_pos;
        if(!m_readNode || capacity == 0) break;

        if(size <= capacity){
            memcpy(buf + readed_size, getReadPos(), size);
            m_readNode->addReadPos(size);
            readed_size += size;
            m_readPos += size;
            size = 0;
        }
        else{
            memcpy(buf + readed_size, getReadPos(), capacity);
            m_readNode->addReadPos(capacity);
            _addReadNode();
            readed_size += capacity;
            m_readPos += capacity;
            size -= capacity;
        }
    }
    printf("读取有效数据 %lubytes\n", readed_size);
}

void ByteArray::read(struct iovec* buf, uint16_t len)
{
    for(int i=0; i<len; i++){
        read(buf[i].iov_base, buf[i].iov_len);
    }
}

void ByteArray::write(const void* buf, size_t size)
{
    if(size <= 0)
        return;

    size_t writed_size = 0;
    while(size > 0){
        if(!m_writeNode) break;
        size_t s = m_writeNode->size - m_writeNode->write_pos;   // 结点缓冲区剩余大小

        if(size < s){
            memcpy(getWritePos(), buf + writed_size, size);
            m_writeNode->addWritePos(size);
            m_writePos += size;
            writed_size += size;
            size = 0;      
        }
        else{
            memcpy(getWritePos(), buf + writed_size, s);
            m_writeNode->addWritePos(s);
            _addWriteNode();
            m_writePos += s;
            writed_size += s;
            size -= s; 
        }
    }
    printf("写入有效数据 %lubytes\n", writed_size);
}

void ByteArray::write(struct iovec* buf, uint16_t len)
{
    for(int i=0; i<len; i++){
        write(buf[i].iov_base, buf[i].iov_len);
    }
}

void ByteArray::clear()
{
    Node* tmp = m_listHeader->next;
    while(tmp){
        Node* next = tmp->next;
        delete tmp;
        tmp = next;
    }
    _init();
}

void ByteArray::clearReaded()
{
    ByteArray::Node* tmp = m_listHeader->next;
    ByteArray::Node* del_Node;
    while(tmp && m_readNode != tmp){
        del_Node = tmp;
        tmp = tmp->next;
        m_listHeader->next = tmp;
        delete del_Node;
        m_readPos += m_readNode->size;
    }
    memset(m_readNode->buf, '\0', m_readNode->read_pos);
}

void ByteArray::addReadedBytes(size_t size)
{
    if(_capacity() < size){
        size = _capacity();
    }
    
    ByteArray::Node* tmp = m_listHeader->next;
    ByteArray::Node* del_Node;
    while(size > 0){
        if(m_readNode->size <= size){
            del_Node = tmp;
            tmp = tmp->next;
            m_listHeader->next = tmp;
            delete del_Node;
            m_readNode = tmp;

            size -= m_readNode->size;
            m_readPos += m_readNode->size; 
        }
        else{
            memset(m_readNode->buf, '\0', size);
            m_readNode->addReadPos(size);
            m_readPos += size;
            size = 0;
        }
    }
}

void ByteArray::addWritedBytes(size_t size)
{
    size_t s = m_writeNode->size - m_writeNode->write_pos;
    while(size > 0){
        if(size < s){
            m_writePos += size;
            m_writeNode->write_pos += size;
            size = 0;
        }
        else{
            m_writePos += s;
            m_writeNode->write_pos += s;
            size -= s;
            _addWriteNode();
        }
    }
}

size_t ByteArray::writeableBytes()
{
    return m_writeNode->size - m_writeNode->write_pos;
}

size_t ByteArray::readableBytes()
{
    return m_writePos - m_readPos;
}

const char* ByteArray::getReadPos()
{
    return m_readNode->buf + m_readNode->read_pos;
}

char* ByteArray::getWritePos()
{
    return m_writeNode->buf + m_writeNode->write_pos;
}

void ByteArray::_init()
{
    m_readPos = m_writePos = 0;
    m_listHeader->next = new Node(_nodeSize());
    m_readNode = m_listHeader->next;
    m_writeNode = m_listHeader->next;
}

size_t ByteArray::_nodeSize()
{
    return m_nodeSize;
}

size_t ByteArray::_capacity()
{
    return m_writePos - m_readPos;
}

void ByteArray::_addReadNode()
{
    if(!m_readNode) return;
    m_readNode = m_readNode->next;
}

void ByteArray::_addWriteNode()
{
    if(m_writeNode){
        m_writeNode->next = new Node(_nodeSize());
        m_writeNode = m_writeNode->next;
    }
    else{
        m_writeNode = new Node(_nodeSize());
    }
}