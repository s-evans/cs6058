cmake_minimum_required(VERSION 2.8.4)

project(aes)

add_executable(aes
    src/main.cpp
    src/keygen.cpp
)

target_link_libraries(aes PRIVATE crypto)

add_executable(test_running_time
    src/keygen.cpp
    src/test_running_time.cpp
)

target_link_libraries(test_running_time PRIVATE crypto)
