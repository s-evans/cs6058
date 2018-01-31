cmake_minimum_required(VERSION 2.8.4)

project(otp)

add_executable(otp src/main.cpp)

target_link_libraries(otp
    boost_system
    boost_filesystem
    )
