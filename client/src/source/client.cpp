#include "../headers/client.h"
#include <iostream>
#include <string>
#include <winsock2.h>
#include <vector>
#include <fstream>
#include <cstdint>
#include "../../../image_ops/src/headers/color.hpp"
#include "../../../image_ops/src/headers/BMPimage.hpp"
#include "../../../image_ops/src/headers/PNGimage.hpp"
#include "../../../image_ops/src/headers/JPEGimage.hpp"


Client::Client(int port, int addrlen, char *ip) {
    this->port = port;
    this->addrlen = addrlen;
    this->ip = ip;
}

void Client::setup() {
    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        std::cout << "Error initializing winsock dll: " << err << std::endl;
        return;
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == INVALID_SOCKET) {
        WSACleanup();
        std::cout << "Error creating socket: " << WSAGetLastError() << std::endl;
        return;
    }

    // Повертаємо неблокуючий режим
    u_long mode = 1;
    ioctlsocket(socket_fd, FIONBIO, &mode);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
}

void Client::connect_to_server() {
    int result = ::connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    if (result == SOCKET_ERROR) {
        std::cout << "Connection to server failed: " << 10 << std::endl;
    }
    std::cout << "Connected to server" << std::endl;
}

void Client::send_message(const std::string& message) {
    std::string msg_with_newline = message + "\n";  // Додаємо \n
    int send_result = send(socket_fd, msg_with_newline.c_str(), msg_with_newline.size(), 0);
    if (send_result == SOCKET_ERROR) {
        std::cerr << "Send message failed" << std::endl;
        closesocket(socket_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Message sent: " << message << std::endl;
    }
}

void Client::run() {
    std::string command;
    char buf[1024];
    
    while (true) {
        std::cout << "\nAvailable commands:\n";
        std::cout << "register\nlogin\nlogout\nget image\nexit\n";
        std::cout << "Enter command: ";
        std::getline(std::cin, command);
        
        if (command == "get image") {
            get_image();
        } else {
            send_message(command);
            
            // Правильна обробка неблокуючого режиму
            bool response_received = false;
            int attempts = 0;
            const int max_attempts = 100; // 10 секунд з інтервалом 100мс
            
            while (!response_received && attempts < max_attempts) {
                int bytes_received = recv(socket_fd, buf, sizeof(buf) - 1, 0);
                
                if (bytes_received > 0) {
                    buf[bytes_received] = '\0';
                    std::string response(buf);
                    std::cout << "Server: " << response << std::endl;
                    response_received = true;
                    
                    // Обробка додаткових запитів
                    if (command == "register" && response.find("Send email") != std::string::npos) {
                        handle_registration();
                    } else if (command == "login" && response.find("Send email") != std::string::npos) {
                        handle_login();
                    }
                } else if (bytes_received == SOCKET_ERROR) {
                    int error = WSAGetLastError();
                    if (error == WSAEWOULDBLOCK) {
                        Sleep(100); // Чекаємо 100мс
                        attempts++;
                    } else {
                        std::cerr << "Recv error: " << error << std::endl;
                        break;
                    }
                } else {
                    std::cout << "Server disconnected" << std::endl;
                    break;
                }
            }
            
            if (!response_received) {
                std::cout << "No response from server (timeout)" << std::endl;
            }
        }
        
        if (command == "exit") break;
    }
}

void Client::handle_registration() {
    std::string email, password, name, surname, note;
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    std::cout << "Name: ";
    std::getline(std::cin, name);
    std::cout << "Surname: ";
    std::getline(std::cin, surname);
    std::cout << "Note: ";
    std::getline(std::cin, note);
    
    std::string data = email + "," + password + "," + name + "," + surname + "," + note;
    
    Sleep(200); // Додаємо затримку для синхронізації
    send_message(data);
    
    // Обробка неблокуючого режиму
    char buf[1024];
    bool response_received = false;
    int attempts = 0;
    const int max_attempts = 50;
    
    while (!response_received && attempts < max_attempts) {
        int bytes_received = recv(socket_fd, buf, sizeof(buf) - 1, 0);
        if (bytes_received > 0) {
            buf[bytes_received] = '\0';
            std::cout << "Server: " << buf << std::endl;
            response_received = true;
        } else if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                Sleep(100);
                attempts++;
            } else {
                std::cerr << "Recv error: " << error << std::endl;
                break;
            }
        }
    }
}

void Client::handle_login() {
    std::string email, password;
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    
    std::string data = email + "," + password;
    
    Sleep(200); // Додаємо затримку для синхронізації
    send_message(data);
    
    // Обробка неблокуючого режиму
    char buf[1024];
    bool response_received = false;
    int attempts = 0;
    const int max_attempts = 50;
    
    while (!response_received && attempts < max_attempts) {
        int bytes_received = recv(socket_fd, buf, sizeof(buf) - 1, 0);
        if (bytes_received > 0) {
            buf[bytes_received] = '\0';
            std::cout << "Server: " << buf << std::endl;
            response_received = true;
        } else if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                Sleep(100);
                attempts++;
            } else {
                std::cerr << "Recv error: " << error << std::endl;
                break;
            }
        }
    }
}

void Client::get_image() {
    send_message("get image");

    // Отримуємо розміри з неблокуючим режимом
    uint32_t width = 0, height = 0;
    
    int received = 0;
    while (received < (int)sizeof(width)) {
        int chunk = recv(socket_fd, reinterpret_cast<char*>(&width) + received, sizeof(width) - received, 0);
        if (chunk > 0) {
            received += chunk;
        } else if (chunk == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                Sleep(10);
                continue;
            } else {
                std::cout << "Error receiving width: " << error << std::endl;
                return;
            }
        } else {
            std::cout << "Connection closed while receiving width\n";
            return;
        }
    }
    
    received = 0;
    while (received < (int)sizeof(height)) {
        int chunk = recv(socket_fd, reinterpret_cast<char*>(&height) + received, sizeof(height) - received, 0);
        if (chunk > 0) {
            received += chunk;
        } else if (chunk == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                Sleep(10);
                continue;
            } else {
                std::cout << "Error receiving height: " << error << std::endl;
                return;
            }
        } else {
            std::cout << "Connection closed while receiving height\n";
            return;
        }
    }

    std::cout << "Image size: " << width << "x" << height << std::endl;

    // Отримуємо пікселі з неблокуючим режимом
    std::vector<uint8_t> raw_pixels(width * height * 4);
    received = 0;
    while (received < (int)raw_pixels.size()) {
        int chunk = recv(socket_fd, reinterpret_cast<char*>(raw_pixels.data() + received), raw_pixels.size() - received, 0);
        if (chunk > 0) {
            received += chunk;
        } else if (chunk == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                Sleep(10);
                continue;
            } else {
                std::cout << "Error receiving pixel data: " << error << std::endl;
                return;
            }
        } else {
            std::cout << "Connection closed while receiving pixel data\n";
            return;
        }
    }

    // Отримуємо повідомлення "Image sent" з неблокуючим режимом
    char buf[1024];
    bool msg_received = false;
    int attempts = 0;
    while (!msg_received && attempts < 50) {
        int bytes_received = recv(socket_fd, buf, sizeof(buf), 0);
        if (bytes_received > 0) {
            msg_received = true;
        } else if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                Sleep(100);
                attempts++;
            } else {
                break;
            }
        }
    }

    std::string fmt, path;
    std::cout << "Choose format to save (bmp/png/jpeg): ";
    std::getline(std::cin, fmt);
    std::cout << "Enter file path: ";
    std::getline(std::cin, path);

    std::unique_ptr<Image> img;
    if (fmt == "bmp") {
        img = std::make_unique<BMPImage>(height, width);
    } else if (fmt == "png") {
        img = std::make_unique<PNGImage>(height, width);
    } else if (fmt == "jpeg" || fmt == "jpg") {
        img = std::make_unique<JPEGImage>(height, width);
    } else {
        std::cout << "Unknown format\n";
        return;
    }

    std::cout << "reading data" << std::endl;
    for (int row = 0; row < (int)height; ++row) {
        for (int col = 0; col < (int)width; ++col) {
            int idx = (row * width + col) * 4;
            Color c(raw_pixels[idx], raw_pixels[idx+1], raw_pixels[idx+2], raw_pixels[idx+3]);
            img->setPixelColor(c, col, row);
        }
    }
    std::cout << "saving" << std::endl;
    try {
        img->save(path);
    } catch (const std::exception& e) {
        std::cerr << "Failed to save image: " << e.what() << std::endl;
        return;
    }
    std::cout << "Image saved!\n";
}

Client::~Client() {
    closesocket(socket_fd);
    WSACleanup();
}