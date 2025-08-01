# pet_project_1

## Pet Project 1: Multiuser Noise Image Generator

### 📋 Project Overview
A client-server application for generating procedural noise images using the Perlin noise algorithm. Multiple clients can connect simultaneously to request custom noise-based images in various formats (BMP, PNG, JPEG). Built for Windows OS using modern C++ and networking technologies.

### 🎯 Purpose
This project demonstrates junior-level software development skills including:

- Network programming and client-server architecture
- Multithreading and concurrent programming
- Database integration and user management
- Image processing and file format handling
- Modern C++ practices and build systems

### 🛠️ Technologies & Tools Used

#### Programming Languages
- **C++20** – Core application logic
- **SQL** – Database queries and user management

#### Networking & Communication
- **WinSock2** – Windows socket programming
- **TCP/IP** – Reliable client-server communication
- **Non-blocking I/O** – Asynchronous network operations

#### Concurrency & Threading
- **std::thread** – Multithreaded server implementation
- **std::mutex** – Thread synchronization
- **std::atomic** – Lock-free operations

#### Database
- **SQLite3** – Lightweight embedded database
- User registration and authentication system
- Persistent data storage

#### Image Processing
- Custom image classes – BMP, PNG, JPEG support
- zlib compression – PNG format implementation
- Perlin Noise Algorithm – Procedural texture generation

#### Build Systems & Tools
- **CMake** – Cross-platform build configuration
- **Makefile** – Traditional build automation
- **Git** – Version control
- **Visual Studio Code** – Development environment

---

### 🏗️ Build Instructions

#### Server Build
```sh
cd server/
make dependencies  # Build SQLite3
make               # Compile server
./program.exe <port> <ip>
```
#### Client Build 
```sh
cd client/
make              # Compile client
./program.exe <port> <ip>
```

#### Image Processing Library
```sh
cd image_ops/
make dependencies  # Build zlib
make              # Compile library
```

### Future Enhancements
<input disabled="" type="checkbox"> Web interface using REST API
<input disabled="" type="checkbox"> Advanced image filters and effects
<input disabled="" type="checkbox"> Image compression optimization
<input disabled="" type="checkbox"> User image galleries and sharing
<input disabled="" type="checkbox"> Docker containerization
<input disabled="" type="checkbox"> Unit testing with Google Test
<input disabled="" type="checkbox"> CI/CD pipeline integration