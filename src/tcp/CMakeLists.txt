add_library(tcp "")

target_link_libraries(tcp
    log
    base 
)

target_sources(tcp
    PRIVATE
        TcpConnection.cpp
        TcpServer.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/TcpConnection.h
        ${CMAKE_CURRENT_LIST_DIR}/TcpServer.h
)

target_include_directories(tcp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

install(TARGETS tcp DESTINATION lib)

set(HEADERS
    TcpConnection.h
    TcpServer.h)

install(FILES ${HEADERS} DESTINATION include/spark)