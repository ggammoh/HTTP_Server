# HTTP Server

A custom HTTP server built from scratch in C as a learning project to understand low-level networking, memory management, and server architecture.

![License](https://img.shields.io/github/license/ggammoh/HTTP_Server)

## Overview

This HTTP server is a learning project developed to deepen understanding of the C programming language and network programming. By implementing a server from scratch, this project explores fundamental concepts like memory management, socket programming, and efficient I/O handling that are often abstracted away in higher-level languages.

The project has two main goals:
- Strengthen C programming skills through practical application
- Gain a deeper understanding of how web servers actually work at a low level

## Features

Current implementation status:

| Feature | Status |
|---------|--------|
| HTTP Request Parsing | ✅ Complete |
| Static File Serving | ✅ Complete |
| Memory Management | ✅ Complete |
| Keep Alive Connections | ✅ Complete |
| Connection Pooling | ✅ Complete |
| Basic Security | 🔄 In Progress |

### Completed Features

- **HTTP Request Parsing**: Correctly parses HTTP headers, methods, and paths according to the HTTP/1.1 specification
- **Static File Serving**: Serves HTML, CSS, JavaScript, and image files from the server's document directory
- **Memory Management**: Careful memory allocation and deallocation to prevent leaks in long-running processes
- **Keep Alive Connections**: Supports persistent connections to reduce the overhead of establishing new TCP connections
- **Connection Pooling**: Efficiently handles multiple concurrent connections using a thread pool

### In Progress
- **Basic Security**: Input validation and error handling to prevent common security issues

## Learning Takeaways

Key insights gained from this project:

- **Low-level HTTP understanding**: Gained practical knowledge of how the HTTP protocol works at a byte level, including the complexities of implementing persistent connections
- **Memory management mastery**: Learned the critical importance of careful allocation and deallocation in long-running C applications
- **Concurrency and performance optimization**: Implemented thread pooling and connection management, providing insights into balancing resource usage with server responsiveness
- **Socket programming and network I/O**: Developed a deep understanding of network communication fundamentals through hands-on implementation
- **Modular C programming**: Applied principles of clean code organization and separation of concerns in a complex, multi-featured application

## Building and Running

### Prerequisites

- GCC compiler
- Make

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/ggammoh/HTTP_Server.git
cd HTTP_Server
```

2. Build the server:
```bash
make
```

3. Run the server:
```bash
./server
```

The server will start and listen on port 8080 by default.

## Project Structure

```
HTTP_Server/
├── include/         # Header files
├── src/             # Source files
├── public/          # Static files to be served
│   ├── css/         # CSS stylesheets
│   ├── index.html   # Homepage
│   └── about.html   # About page
├── Makefile         # Build configuration
└── server           # Compiled binary
```

## Development Roadmap

This project is an ongoing learning experience with the following roadmap:

1. **Phase 1 (Completed)**: Basic HTTP Server
   - Implemented socket programming, basic request handling, and response generation

2. **Phase 2 (Completed)**: Performance Optimization
   - Working on concurrency, connection pooling, and efficient resource utilization
### Potential Next Steps
1. **Phase 3 (TBD)**: Advanced Features
   - Add support for HTTPS, WebSockets, and more sophisticated routing

2. **Phase 4 (TBD)**: Production Readiness
   - Focus on reliability, logging, monitoring, and deployment configurations


## License

This project is licensed under the terms of the included LICENSE file.

## Acknowledgments

- This project is purely educational and not intended for production use.
- Inspired by the desire to understand the inner workings of web servers and learn lower-level programming concepts for future systems programming projects
 
