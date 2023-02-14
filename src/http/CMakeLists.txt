add_library(http "")

target_link_libraries(http
    log
    tcp
    timer
    event
    base
)

target_sources(http
    PRIVATE
        HttpConn.cpp
        HttpRequest.cpp
        HttpResponse.cpp
        HttpServer.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/HTTP.h
        ${CMAKE_CURRENT_LIST_DIR}/HttpConn.h
        ${CMAKE_CURRENT_LIST_DIR}/HttpRequest.h
        ${CMAKE_CURRENT_LIST_DIR}/HttpResponse.h
        ${CMAKE_CURRENT_LIST_DIR}/HttpServer.h
)

target_include_directories(http
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)