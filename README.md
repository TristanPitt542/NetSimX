## NetSimX ##
NetSimX is a lightweight, C++ network simulation project designed to demonstrate packet transfer between nodes, threading, and basic logging. It is modular, easy to extend, and ideal as a showcase for C++ network programming concepts.

## Features ##

 - Simulates network nodes exchanging packets
 - Thread-safe message handling between nodes
 - Timestamped packet latency measurement
 - Simple logging system using std::cout
 - Fully modular and extensible C++ codebase

## Structure ##

NetSimX/
 - CMakeLists.txt
 - include/
 - - Logger.h
 - - Node.h
 - - Packet.h
 - src/
 - - main.cpp
 - - Logger.cpp
 - - Node.cpp
 - - Packet.cpp

## Build ##

Using CMD:
 1. git close https://github.com/TristanPitt542/NetSimX.git
 2.cd NetSimX
 3. cmake -B build
 4. cmake --build build --config Release
 5. .\build\Release\NetSimX.exe

Using VS Code:
 1. Open Project in VS Code
 2. Make sure CMake Tools & C/C++ is installed
 3. CTRL + SHIFT + P
 4.CMake: configure
 5. Cmake: Build
 6. Run
