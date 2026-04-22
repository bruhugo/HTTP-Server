set(NETWORK_SOURCES
    src/Connection.cpp
)

add_library(network ${NETWORK_SOURCES})

target_include_directories(network PUBLIC 
    ${CMAKE_INCLUDE_CURRENT_DIR}
)

target_link_libraries(network PUBLIC basic)
