#include "threadpool.h"

ThreadPool::ThreadPool(const std::string& poolName, int maxSize)
    : m_poolName(poolName), m_maxSize(maxSize)
    , m_isRunning(false)
{

}

ThreadPool::~ThreadPool()
{
    if(m_isRunning){
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(numThreads);
    m_threads.reserve(numThreads);
    m_isRunning = true;

    for(int i=0; i<numThreads; i++){
        char id[32];
        snprintf(id, sizeof(id), "thread_%d", i);
        Thread *thread = new Thread(std::bind(
            &ThreadPool::runInThread, this), id);
        thread->detach();
        m_threads.emplace_back(thread);
    }
}

void ThreadPool::stop()
{
    std::lock_guard<std::mutex> locker(m_mutex);
    m_isRunning = false;
    m_cond.notify_all();
}

void ThreadPool::addTask(Task task)
{
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_queue.push_back(task);
    }
    m_cond.notify_one();
}

void ThreadPool::runInThread()
{
    while(m_isRunning){
        std::unique_lock<std::mutex> locker(m_mutex);
        if(!m_queue.empty()){
            Task task = m_queue.front();
            m_queue.pop_front();
            locker.unlock();
            task();
            locker.lock();
        }
        else{
            m_cond.wait(locker);
        }
    }
}

bool ThreadPool::isRunning()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_isRunning == true;
}

void ThreadPool::setMaxSize(int size)
{
    assert(size > 0);
    std::lock_guard<std::mutex> locker(m_mutex);
    m_maxSize = size;
}

int ThreadPool::getPoolSize() const 
{
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_threads.size(); 
}

int ThreadPool::getTaskSize() const 
{
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_queue.size(); 
}
