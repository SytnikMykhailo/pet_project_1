#include "../headers/client.h"
#include <iostream>
#include <string>
#include <winsock2.h>

Client::Client(int port, int addrlen, char *ip) {
    this->port = port;
    this->addrlen = addrlen;
    this->ip = ip;
}

void Client::setup() {
    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        std::cout << "Error initializing winsock dll: 10" << std::endl;
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == INVALID_SOCKET) {
        WSACleanup();
        std::cout << "Error creating socket: 10" << std::endl;
    }

    mode = 1;
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
    int send_result = send(socket_fd, message.c_str(), message.size(), 0);
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
    std::string message;
    while (true) {
        std::cout << "Enter message to send: ";
        std::getline(std::cin, message);
        send_message(message);
        if (message == "exit") break;
    }
}

Client::~Client() {
    closesocket(socket_fd);
    WSACleanup();
}