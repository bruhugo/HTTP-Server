add_executable(simple_test example.cpp)

target_link_libraries(simple_test PUBLIC basic)
target_link_libraries(simple_test PUBLIC network)