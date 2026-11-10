# NetSimX — Multi-Threaded Asynchronous Network Simulator (C++)

NetSimX is a modular, high-performance C++ networking simulation library and test harness built with Asio. Designed to demonstrate fundamental low-level systems engineering and networking concepts, it features custom TCP stream-framing, multi-threaded event-driven I/O context loops, real-time packet latency telemetry, and thread-safe console logging.

## 🚀 Key Architectural Features

* **Asynchronous Networking Core:** Driven by `asio::io_context` event loops executing on dedicated background worker threads, stabilized by `executor_work_guard` to manage context lifetimes.
* **Stream-Delimited TCP Framing:** Custom line-delimited payload serialization (`|` and `\n`) handled via asynchronous read stream buffers (`asio::async_read_until`) to eliminate packet fragmentation and truncation issues.
* **Thread-Safe Static Logging:** Centralized mutex-serialized output system (`Logger`) preventing console log interleaving across concurrent asynchronous handler executions.
* **Latency Telemetry & Metrics:** Real-time round-trip latency evaluation calculated using microsecond epoch timestamps embedded within transmitted packet headers.
* **Dual Execution Modes:**
  * **Default (Zero Arguments):** Automated 2-node loopback integration demo for rapid local verification.
  * **Parameterized CLI:** Multi-process mode allowing nodes to run as standalone network endpoints with configurable host and port bindings.

## 🔨 Build & Run Instructions

Using CMD:
1. git clone https://github.com/TristanPitt542/NetSimX.git
2. cd NetSimX
3. cmake -B build
4. cmake --build build --config Release
5. .\build\Release\NetSimX.exe

Using VS Code:
1. Open Project in VS Code
2. Make sure CMake Tools & C/C++ is installed
3. CTRL + SHIFT + P
4. CMake: configure
5. CMake: Build
6. Run
