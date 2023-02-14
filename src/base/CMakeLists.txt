add_library(base "")

target_link_libraries(base
    PRIVATE
        log
)

target_sources(base
    PRIVATE
        Buffer.cpp
        Socket.cpp
        SocketsOps.cpp
        thread.cpp
        threadpool.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Buffer.h
        ${CMAKE_CURRENT_LIST_DIR}/CurrentThread.h
        ${CMAKE_CURRENT_LIST_DIR}/Socket.h
        ${CMAKE_CURRENT_LIST_DIR}/SocketsOps.h
        ${CMAKE_CURRENT_LIST_DIR}/thread.h
        ${CMAKE_CURRENT_LIST_DIR}/threadpool.h
)

target_include_directories(base
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)