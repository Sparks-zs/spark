#include "thread.h"

__thread int CurrentThread::t_cachedTid = 0;

void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
  }
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_id(CurrentThread::tid()), m_cb(cb), m_name(name)
{
    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(ret != 0){
        LOG_ERROR << "Failed to create new thread";
    }
}

Thread::~Thread()
{
}

pid_t Thread::gettid() const
{
    return m_id;
}

const std::string& Thread::getName() const
{
    return m_name;
}

void Thread::detach()
{
    pthread_detach(m_thread);
}

void Thread::join()
{
    int ret = pthread_join(m_thread, nullptr);
    if(ret != 0){
        switch(ret){
            case EDEADLK:
                LOG_ERROR << "thread join() fail, error code: EDEADLK";
                break;
            case EINVAL:
                LOG_ERROR << "thread join() fail, error code: EINVAL";
                break;
            case ESRCH:
                LOG_ERROR << "thread join() fail, error code: ESRCH";
                break;
        }
    }
}

void* Thread::run(void* obj)
{
    Thread* thread = (Thread*)obj;
    thread->m_cb();
    
    return nullptr;
}