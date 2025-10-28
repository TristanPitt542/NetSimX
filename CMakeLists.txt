cmake_minimum_required(VERSION 3.15)
project(NetSimX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch Asio header-only (modern way)
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG master
)

# This replaces the old FetchContent_Populate() call
FetchContent_MakeAvailable(asio)

add_executable(NetSimX
    src/main.cpp
    src/Logger.cpp
    src/Packet.cpp
    src/NetNode.cpp
)

# include directories
target_include_directories(NetSimX PRIVATE
    include
    ${asio_SOURCE_DIR}/asio/include
)

# Define ASIO_STANDALONE
target_compile_definitions(NetSimX PRIVATE ASIO_STANDALONE)

if (WIN32)
  target_compile_definitions(NetSimX PRIVATE _WIN32_WINNT=0x0A00)
  target_link_libraries(NetSimX PRIVATE ws2_32)
endif()
