add_library(filemanager "")

target_link_libraries(filemanager
    log 
    http
    base
)

target_sources(filemanager
    PRIVATE
        filemanager.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/filemanager.h
)

target_include_directories(filemanager
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)