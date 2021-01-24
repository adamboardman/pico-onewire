cmake_minimum_required(VERSION 3.12)

add_definitions(-DMOCK_PICO_PI)

project(tests)

set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 REQUIRED)

add_executable(tests tests.cpp test_one_wire.cpp pico_pi_mocks.cpp ../source/one_wire.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(tests)