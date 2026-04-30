set(NETWORK_SOURCES
    src/Connection.cpp
    src/Header.cpp
    src/Request.cpp
    src/Server.cpp
    src/Types.cpp
    src/Response.cpp
    src/Context.cpp
    src/RadixTree.cpp
)

add_library(network ${NETWORK_SOURCES})

target_include_directories(network PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(network PUBLIC basic)
