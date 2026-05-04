add_executable(static static.cpp)

target_link_libraries(static PUBLIC basic)
target_link_libraries(static PUBLIC network)