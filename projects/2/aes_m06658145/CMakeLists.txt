cmake_minimum_required(VERSION 2.8.4)

project(aes)

add_executable(aes
    src/main.cpp
    src/keygen.cpp
    src/aes.cpp
)

add_executable(test_frequency
    src/aes.cpp
    src/keygen.cpp
    src/test_frequency.cpp
)

add_executable(test_running_time
    src/aes.cpp
    src/keygen.cpp
    src/test_running_time.cpp
)
