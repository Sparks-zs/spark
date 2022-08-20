#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <assert.h>
#include <string>

#include "thread.h"

class ThreadPool
{
public:
    typedef std::function<void()> Task;

    ThreadPool(const std::string& poolName, int maxSize=5);
    ~ThreadPool();

    void start(int numThreads);
    void stop();
    void addTask(Task task);

    bool isRunning();
    void setMaxSize(int size);
    int getPoolSize() const;
    int getTaskSize() const;

private:
    void runInThread();

    std::string m_poolName;
    int m_maxSize;
    bool m_isRunning;
    
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;
    std::deque<Task> m_queue;
    std::vector<std::unique_ptr<Thread>> m_threads;
};

#endif // THREADPOOL_H