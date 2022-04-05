add_library(pico-onewire-lib INTERFACE)
target_include_directories(pico-onewire-lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(pico-onewire-lib INTERFACE hardware_flash)
target_sources(pico-onewire-lib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/source/one_wire.cpp
    ${CMAKE_CURRENT_LIST_DIR}/api/one_wire.h
)

