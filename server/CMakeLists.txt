cmake_minimum_required(VERSION 3.16)

project(must-server)

find_library(MODBUS_LIBRARY modbus)
if(NOT MODBUS_LIBRARY)
    message(FATAL_ERROR "libmodbus not found. Install with: sudo apt-get install libmodbus-dev")
endif()

add_executable(must-server
    src/main.cpp
    src/modbus_reader.cpp
    src/modbus_reader.h
    src/udp_sender.cpp
    src/udp_sender.h
    src/config.cpp
    src/config.h
    src/inverter_data.h
)

target_link_libraries(must-server
    ${MODBUS_LIBRARY}
    pthread
)

target_include_directories(must-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

install(TARGETS must-server DESTINATION bin)
