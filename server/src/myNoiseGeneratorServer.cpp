#include <iostream>
#include "headers/server.hpp"


Server *server = nullptr;

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        std::cout << "Ctrl+C pressed" << std::endl;
        if (server != nullptr) {
            closesocket(server->socket_fd);
            WSACleanup();
            delete server;
        }
        ExitProcess(0);
    }
    return TRUE;
}

int main(int argc, char** argv){
    if(argc == 4) return 1;

    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cerr << "Failed to set control handler" << std::endl;
        return -1;
    }

    std::cout << "Hello world! " << argv[0] << std::endl;
    
    int port = atoi(argv[1]);
    if(port < 1024 || port > 49151){
        std::cerr << "Wrong port number" << std::endl;
        return -1;
    }
    server = new Server(port, sizeof(sockaddr_in), 1, argv[2]);
    std::cout << "Server created" << std::endl;
    server->setup();
    std::cout << "Server running" << std::endl;
    server->run();
    delete server;
    return 0;
}