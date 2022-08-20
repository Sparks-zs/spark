#ifndef HEAPTIMER_H
#define HEAPTIMER_H

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include <chrono>

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode {
    int id;
    TimeStamp expires;          // 定时器绝对时间
    TimeoutCallBack cb;         // 回调函数
    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};
class HeapTimer {               // 时间堆，以最小超时时间为心跳时间
public:
    HeapTimer() { heap_.reserve(64); }

    ~HeapTimer() { clear(); }
    
    void adjust(int id, int newExpires);        // 增加结点的超时时间

    void add(int id, int timeOut, const TimeoutCallBack& cb);

    void doWork(int id);

    void clear();

    void tick();

    void pop();

    int GetNextTick();      // 清除超时结点，并返回下一个结点的超时时间

private:
    void del_(size_t i);    // 删除结点
    
    void siftup_(size_t i);         // 向上调整

    bool siftdown_(size_t index, size_t n);     // 向下调整

    void SwapNode_(size_t i, size_t j);     // 交换结点i和j

    std::vector<TimerNode> heap_;       // 小根堆

    std::unordered_map<int, size_t> ref_;   // 结点编号对应结点加入小根堆时的size
};


#endif // HEAPTIMER_H