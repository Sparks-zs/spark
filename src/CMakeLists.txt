add_executable(spark main.cpp)

add_subdirectory(base)
add_subdirectory(log)
add_subdirectory(http)
add_subdirectory(tcp)
add_subdirectory(utils)

target_link_libraries(spark
    PRIVATE
        http
        tcp
        log
        base     
        utils
)
