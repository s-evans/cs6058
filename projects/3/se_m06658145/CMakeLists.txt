cmake_minimum_required(VERSION 2.8.4)

project(se)

add_executable(se
    src/main.cpp
    src/keygen.cpp
)

target_link_libraries(se PRIVATE crypto boost_filesystem boost_system)

add_executable(test_running_time
    src/keygen.cpp
    src/test_running_time.cpp
)

target_link_libraries(test_running_time PRIVATE crypto boost_filesystem boost_system)
