add_library(pico_one_wire INTERFACE)

target_sources(pico_one_wire INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/source/one_wire.cpp
        )

target_include_directories(pico_one_wire INTERFACE ${CMAKE_CURRENT_LIST_DIR}/api)
target_link_libraries(pico_one_wire INTERFACE pico_stdlib hardware_gpio)