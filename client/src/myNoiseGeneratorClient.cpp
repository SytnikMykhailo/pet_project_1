#include <iostream>
#include "headers/client.h"

int main(int argc, char **argv){

    if (argc != 3) {
        std::cout << "Usage: client <IP> <PORT>: " << 10 << std::endl;
    }

    int port = atoi(argv[1]);
    if (port < 1024 || port > 49151) {
        std::cout << "Invalid  port number: 10" << std::endl;
    }

    Client client(port, sizeof(sockaddr_in), argv[2]);
    client.setup();
    client.connect_to_server();
    client.run();

    return 0;
}