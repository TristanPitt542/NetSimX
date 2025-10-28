## NetSimX ##
NetSimX is a lightweight, C++ network simulation project designed to demonstrate packet transfer between nodes, threading, and basic logging. It is modular, easy to extend, and ideal as a showcase for C++ network programming concepts.

## Features ##

*Simulates network nodes exchanging packets
*Thread-safe message handling between nodes
*Timestamped packet latency measurement
*Simple logging system using std::cout
*Fully modular and extensible C++ codebase

## Structure ##

NetSimX/
    CMakeLists.txt
    include/
        Logger.h
        Node.h
        Packet.h
    src/
        main.cpp
        Logger.cpp
        Node.cpp
        Packet.cpp

## Build ##

Using CMD:
 - git close https://github.com/TristanPitt542/NetSimX.git
 - cd NetSimX
 - cmake -B build
 - cmake --build build --config Release
 - .\build\Release\NetSimX.exe

Using VS Code:
 - Open Project in VS Code
 - Make sure CMake Tools & C/C++ is installed
 - CTRL + SHIFT + P
 - CMake: configure
 - Cmake: Build
 - Run