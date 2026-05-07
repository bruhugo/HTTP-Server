# High-Performance C++ HTTP Server

A lightweight, high-performance HTTP server implementation in C++17, designed for understanding HTTP/1.1 fundamentals and exploring efficient networking patterns.

## Features

- **Asynchronous I/O**: Built on `epoll` for efficient connection handling on Linux.
- **Multi-threaded**: Integrated `ThreadPool` to handle requests concurrently.
- **Robust Routing**: Radix Tree-based router supporting:
  - Standard HTTP methods: `GET`, `POST`, `PUT`, `PATCH`, `DELETE`.
  - Dynamic route parameters (e.g., `/user/:id`).
  - Middleware support with chainable execution (`ctx.next()`).
- **HTTP/1.1 Support**:
  - Full request/response lifecycle.
  - Header and Cookie management.
  - Query parameter parsing.
- **Advanced Capabilities**:
  - **Caching**: Built-in LRU Cache for optimized response delivery.
  - **Static Files**: Serve static assets with MIME type support.
  - **Logging**: Configurable multi-level logging system.

## Project Structure

The project is organized into two primary namespaces:

- `server::basic`: Infrastructure components like `ThreadPool` and `Logger`.
- `server::network`: Core HTTP implementation including `Server`, `Request`, `Response`, `RadixTree`, and `Cache`.

### Basic Example

```cpp
#include "Server.hpp"
#include "Logger.hpp"

using namespace server::network;

int main() {
    server::basic::Logger::setLogLevel("DEBUG");
    Server server(20); // Initialize with 20 threads

    // Middleware example
    server.use("/", [](Context& ctx) {
        ctx.next();
    });

    // Simple JSON route
    server.get("/health", [](Context& ctx) {
        ctx.res.JSON(200, "{\"status\": \"active\"}");
    });

    // Route parameters
    server.get("/echo/:msg", [](Context& ctx) {
        ctx.res.setBody(ctx.params.at("msg"));
    });

    server.listenPort("8080");
    return 0;
}
```

### Building the Project

Ensure you have CMake installed.

```bash
mkdir build && cd build
cmake ..
make
```

Run the example:
```bash
./examples/simple/simple_example
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
