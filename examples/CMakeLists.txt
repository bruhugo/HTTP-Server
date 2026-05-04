option(SIMPLE_EXAMPLE "build simple example" OFF)
if(SIMPLE_EXAMPLE)
    add_subdirectory(simple)
endif()

option(STATIC_EXAMPLE "build static example" OFF)
if(STATIC_EXAMPLE)
    add_subdirectory(static)
endif()