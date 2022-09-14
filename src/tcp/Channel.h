#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

class EventLoop;

class Channel
{
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();

    void setReadCallback(EventCallback cb){
        _readCallback = std::move(cb); 
    }

    void setWriteCallback(EventCallback cb){
        _writeCallback = std::move(cb);
    }
    
    void setCloseCallback(EventCallback cb){
        _closeCallback = std::move(cb);
    }
    void setErrorCallback(EventCallback cb){
        _errorCallback = std::move(cb);
    }
    
    void enableReading() { _events |= kReadEvent; _update(); }
    void disableReading() { _events &= ~kReadEvent; _update(); }
    void enableWriting() { _events |= kWriteEvent; _update(); }
    void disableWriting() { _events &= ~kWriteEvent; _update(); }
    void disableAll() { _events = kNoneEvent; _update(); }
    bool isWriting() const { return _events & kWriteEvent; }
    bool isReading() const { return _events & kReadEvent; }
    bool isNoneEvent() const { return _events & kNoneEvent; }
    
    EventLoop* ownerLoop(){ return _loop; }

    int events() { return _events; }
    
    void set_revents(const int events){ _revents = events; }
    
    int fd() const { return _fd; }

    int index() { return _index; }
    void setIndex(int i) { _index = i; }

    void remove();

private:
    void _update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* _loop;
    const int _fd;
    int _events;    // 可处理事件
    int _revents;   // 目前活动事件
    int _index;     // used for epoller

    EventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _closeCallback;
    EventCallback _errorCallback;
};


#endif // CHANNEL_H