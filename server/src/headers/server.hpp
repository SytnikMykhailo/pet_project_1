#include <winsock2.h>

#include <iostream>
#include <winsock2.h>
#include "../errors/errors.h"
#include <signal.h>
#include <vector>
#include "database.hpp"

void handle_signal(int signal);

class Server{
public:
    Server() = default;
    Server(int port, int addrlen, int opt, char *ip);
    void run();
    void setup();
    friend void handle_signal(int signal);
    sockaddr_in get_server_address(){
        return server_address;
    }
    ~Server();
private:
    Database db;
    SOCKET socket_fd;
    int port;
    struct sockaddr_in server_address;
    int addrlen;
    int opt;
    WSAData wsaData;
    WORD wVersionRequested;
    u_long mode;
};