add_library(event "")

target_link_libraries(event
    log
    base 
    timer
)

target_sources(event
    PRIVATE
        Acceptor.cpp
        Channel.cpp
        epoller.cpp
        EventLoop.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Acceptor.h
        ${CMAKE_CURRENT_LIST_DIR}/Channel.h
        ${CMAKE_CURRENT_LIST_DIR}/epoller.h
        ${CMAKE_CURRENT_LIST_DIR}/EventLoop.h
        ${CMAKE_CURRENT_LIST_DIR}/EventLoopThread.h
        ${CMAKE_CURRENT_LIST_DIR}/EventLoopThreadPool.h
)

target_include_directories(event
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)