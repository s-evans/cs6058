cmake_minimum_required(VERSION 2.8.4)

project(pow)

add_executable(pow
    src/main.cpp
)

target_link_libraries(pow PRIVATE crypto)

add_executable(test_running_time
    src/test_running_time.cpp
)

target_link_libraries(test_running_time PRIVATE crypto)
