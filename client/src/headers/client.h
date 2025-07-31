#include <iostream>
#include <winsock2.h>
#include <vector>

class Client {
public:
    Client(int port, int addrlen, char *ip);
    void setup();
    void connect_to_server();
    void send_message(const std::string& message);
    void run();
    ~Client();
    void get_image();

private:
    void handle_registration();
    void handle_login();
    std::string receive_response();
    bool receive_exact(char* buffer, size_t size);
private:
    SOCKET socket_fd;
    struct sockaddr_in server_address;
    int port;
    int addrlen;
    char *ip;
    WSAData wsaData;
    WORD wVersionRequested;
    u_long mode;
};