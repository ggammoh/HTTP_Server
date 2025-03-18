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
| Static File Serving | ✅ Complete |
| HTTP Request Parsing | ✅ Complete |
| Memory Management | ✅ Complete |
| Connection Pooling | 🔄 In Progress |
| Modular Architecture | 🔄 In Progress |
| Basic Security | 🔄 In Progress |

### Completed Features

- **Static File Serving**: Serves HTML, CSS, JavaScript, and image files from the server's public directory
- **HTTP Request Parsing**: Correctly parses HTTP headers, methods, and paths according to the HTTP/1.1 specification
- **Memory Management**: Careful memory allocation and deallocation to prevent leaks in long-running processes

### In Progress

- **Connection Pooling**: Efficiently handles multiple concurrent connections using a thread pool
- **Modular Architecture**: Clean separation of concerns for easier maintenance and extensibility
- **Basic Security**: Input validation and error handling to prevent common security issues

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

2. **Phase 2 (Current)**: Performance Optimization
   - Working on concurrency, connection pooling, and efficient resource utilization

3. **Phase 3 (Planned)**: Advanced Features
   - Will add support for HTTPS, WebSockets, and more sophisticated routing

4. **Phase 4 (Planned)**: Production Readiness
   - Will focus on reliability, logging, monitoring, and deployment configurations

## Learning Takeaways

Key insights gained from this project:

- The importance of manual memory management and how to avoid common pitfalls
- How the HTTP protocol works at a byte level, not just conceptually
- The intricacies of socket programming and network I/O
- How to design clean, modular C code for a complex application

## License

This project is licensed under the terms of the included LICENSE file.

## Acknowledgments

- This project is purely educational and not intended for production use.
- Inspired by the desire to understand the inner workings of web servers.
 
