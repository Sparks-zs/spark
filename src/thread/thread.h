#ifndef THREAD_H
#define THREAD_H

#include <string>
#include <functional>
#include <pthread.h>
#include <sys/syscall.h>

#include "CurrentThread.h"
#include "../log/Log.h"

class Thread
{
public:
    Thread(std::function<void()>cb, const std::string& name);
    ~Thread();

    pid_t gettid() const;

    const std::string& getName() const;

    void detach();
    void join();

    static void* run(void* obj);

private:
    pid_t m_id;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;
};

#endif // THREAD_H