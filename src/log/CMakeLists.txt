add_library(log "")

target_link_libraries(log
    base
)

target_sources(log
    PRIVATE
        Log.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/BlockQueue.h
        ${CMAKE_CURRENT_LIST_DIR}/Log.h
)

target_include_directories(log
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ../${CMAKE_CURRENT_LIST_DIR}/base
)

install(TARGETS log DESTINATION lib)

set(HEADERS
    BlockQueue.h
    Log.h)

install(FILES ${HEADERS} DESTINATION include/spark)