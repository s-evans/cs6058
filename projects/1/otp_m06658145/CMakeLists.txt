cmake_minimum_required(VERSION 2.8.4)

project(otp)

add_executable(otp
    src/main.cpp
    src/keygen.cpp
)

add_executable(test_keygen
    src/keygen.cpp
    src/test_keygen.cpp
)
