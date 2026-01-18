cmake_minimum_required(VERSION 3.16)
project(MustConnect VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set debug build type by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Enhanced debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG -ggdb -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG -ggdb -fno-omit-frame-pointer")

# Options
option(BUILD_SERVER "Build Modbus server" ON)

# Add subdirectories
if(BUILD_SERVER)
    add_subdirectory(server)
endif()
