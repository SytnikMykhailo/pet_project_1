#pragma once
#include <winsock2.h>
#include <iostream>
#include <signal.h>
#include <vector>      // Додаємо для std::vector
#include <cstdint>     // Додаємо для uint8_t
#include <string>      // Додаємо для std::string
#include "database.hpp"

class Server{
public:
    Server() = default;
    Server(int port, int addrlen, int opt, char *ip);
    virtual void run();
    void setup();
    friend BOOL WINAPI ConsoleHandler(DWORD signal);
    sockaddr_in get_server_address(){
        return server_address;
    }
    virtual ~Server();

    struct ClientState {
        enum State { 
            IDLE, 
            WAITING_REGISTER_DATA, 
            WAITING_LOGIN_DATA,
            SENDING_IMAGE_HEADER,
            SENDING_IMAGE_DATA 
        };
        State state = IDLE;
        std::string buffer; // Буфер для накопичення даних
        std::vector<uint8_t> image_data; // Для відправки зображень
        size_t bytes_sent = 0;
    };

private:
    bool handle_client_read(SOCKET s);
    void handle_client_write(SOCKET s);
    bool handle_message_stateful(SOCKET s, const std::string& message);
    bool handle_command(SOCKET s, const std::string& message, ClientState& state);
    bool handle_registration_data(SOCKET s, const std::string& data, ClientState& state);
    bool handle_login_data(SOCKET s, const std::string& data, ClientState& state);
    void generate_and_queue_image(SOCKET s, ClientState& state);

protected:
    SOCKET socket_fd;
    Database *db;
private:
    int port;
    struct sockaddr_in server_address;
    int addrlen;
    int opt;
    WSAData wsaData;
    WORD wVersionRequested;
    u_long mode;
};