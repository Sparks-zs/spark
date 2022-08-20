#include "Buffer.h"

ssize_t Buffer::readFd(int sockFd, int* saveErrno)
{
    char buff[65536];
    struct iovec iov[2];
    const size_t writable = writeableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = _begin() + m_writePos;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(sockFd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        m_writePos += len;
    }
    else {
        m_writePos = m_buf.size();
        append(buff, len - writable);
    }
    return len;
}

