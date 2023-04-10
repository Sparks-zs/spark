add_library(utils "")

target_sources(utils
    PRIVATE
        sfile.cpp
        base64.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/sfile.h
        ${CMAKE_CURRENT_LIST_DIR}/base64.h
)

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

install(TARGETS utils DESTINATION lib)

set(HEADERS
    sfile.h
    base64.h
)

install(FILES ${HEADERS} DESTINATION include/spark)