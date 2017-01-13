cmake_minimum_required(VERSION 2.8)
project(MicroBitOneWireTest)

include_directories(.)
include_directories(../api)

add_definitions(-DMOCK_MICROBIT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_BUILD_TYPE Debug)

set(SOURCE_FILES ../api/OneWire.h)
add_library(OneWire ../source/OneWire.cpp ../api/OneWire.h MicroBitMocks.h)

find_package(CxxTest)
enable_testing()
CXXTEST_ADD_TEST(OneWireTest OneWireTest.cpp ${CMAKE_CURRENT_SOURCE_DIR}/OneWireTest.h MicroBitMocks.cpp MicroBitMocks.h)

target_link_libraries(OneWireTest OneWire)

