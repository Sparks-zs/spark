add_library(timer "")

target_link_libraries(timer
    log
    event
    base
)

target_sources(timer
    PRIVATE
        Timer.cpp
        TimerQueue.cpp
        TimerStamp.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Timer.h
        ${CMAKE_CURRENT_LIST_DIR}/TimerQueue.h
        ${CMAKE_CURRENT_LIST_DIR}/TimerStamp.h
)

target_include_directories(timer
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
