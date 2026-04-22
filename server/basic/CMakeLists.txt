set(BASIC_SOURCES
    src/ThreadPool.cpp
)

add_library(basic ${BASIC_SOURCES})

target_include_directories(basic PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)