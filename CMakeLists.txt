cmake_minimum_required(VERSION 3.15)
project(NetSimX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory for headers
include_directories(include)

# Source files
add_executable(NetSimX
    src/main.cpp
    src/Logger.cpp
    src/Node.cpp
    src/Packet.cpp
)

target_include_directories(NetSimX PRIVATE include)
set_target_properties(NetSimX PROPERTIES CXX_STANDARD 20)
