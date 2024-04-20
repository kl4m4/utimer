cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(utimer
    LANGUAGES C CXX
)

add_library(utimer STATIC
    source/utimer.c
    include/utimer/utimer.h
)
target_include_directories(utimer PUBLIC
    include
)

add_executable(utimer_demo
    source/demo.c
)

target_link_libraries(utimer_demo PUBLIC utimer)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()