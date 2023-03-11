add_library(base "")

target_sources(base
    PRIVATE
        Buffer.cpp
        Socket.cpp
        SocketsOps.cpp
        thread.cpp
        threadpool.cpp
        Acceptor.cpp
        Channel.cpp
        epoller.cpp
        EventLoop.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        Timer.cpp
        TimerQueue.cpp
        TimerStamp.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Buffer.h
        ${CMAKE_CURRENT_LIST_DIR}/CurrentThread.h
        ${CMAKE_CURRENT_LIST_DIR}/Socket.h
        ${CMAKE_CURRENT_LIST_DIR}/SocketsOps.h
        ${CMAKE_CURRENT_LIST_DIR}/thread.h
        ${CMAKE_CURRENT_LIST_DIR}/threadpool.h
        ${CMAKE_CURRENT_LIST_DIR}/Acceptor.h
        ${CMAKE_CURRENT_LIST_DIR}/Channel.h
        ${CMAKE_CURRENT_LIST_DIR}/epoller.h
        ${CMAKE_CURRENT_LIST_DIR}/EventLoop.h
        ${CMAKE_CURRENT_LIST_DIR}/EventLoopThread.h
        ${CMAKE_CURRENT_LIST_DIR}/EventLoopThreadPool.h
        ${CMAKE_CURRENT_LIST_DIR}/Timer.h
        ${CMAKE_CURRENT_LIST_DIR}/TimerQueue.h
        ${CMAKE_CURRENT_LIST_DIR}/TimerStamp.h
)

target_include_directories(base
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../log
)

install(TARGETS base DESTINATION lib)

set(HEADERS
    Buffer.h
    CurrentThread.h
    Socket.h
    SocketsOps.h
    thread.h
    threadpool.h
    Acceptor.h
    Channel.h
    epoller.h
    EventLoop.h
    EventLoopThread.h
    EventLoopThreadPool.h
    Timer.h
    TimerQueue.h
    TimerStamp.h)

install(FILES ${HEADERS} DESTINATION include/spark)