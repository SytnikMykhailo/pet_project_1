#include "../headers/server.hpp"
#include "../headers/hash_table.hpp"
#include <filesystem>


Server::~Server(){
    WSACleanup();
    closesocket(socket_fd);
    delete db;
}

Server::Server(int port, int addrlen, int opt, char *ip){
    this->port = port;
    this->addrlen = addrlen;
    this->opt = opt;
    this->server_address.sin_family = AF_INET;
    this->server_address.sin_port = htons(port);
    this->server_address.sin_addr.s_addr = inet_addr(ip);
    this->db = new Database("users.db");
}

void Server::setup(){
    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0){
        std::cerr << "Error initializing winsock dll" << std::endl;
        return;
    }
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == INVALID_SOCKET){
        WSACleanup();
        std::cerr << "Error creating socket" << std::endl;
        return;
    }
    mode = 1;
    ioctlsocket(socket_fd, FIONBIO, &mode);

    if(bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1){
        std::cerr << "Error binding socket" << std::endl;
        return;
    }

    if(listen(socket_fd, 10) == -1){
        std::cerr << "Error listening socket" << std::endl;
        return;
    }

    if (!std::filesystem::exists(db->get_db_name())) {
        std::cout << "Database file not found. Creating database and table..." << std::endl;
        Database::create_db(db->get_db_name());

        db->open_db(db->get_db_name());
        db->create_table();
    } else {
        db->open_db(db->get_db_name());
    }
}

void Server::run() {
    std::vector<SOCKET> client_sockets;
    fd_set readfds;

    HashTable<SOCKET> connectedUsers;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);
        SOCKET max_sd = socket_fd;

        for (SOCKET s : client_sockets) {
            FD_SET(s, &readfds);
            if (s > max_sd) {
                max_sd = s;
            }
        }
        int activity = select(0, &readfds, NULL, NULL, NULL);
        if (activity == SOCKET_ERROR) {
            std::cerr << "Select error: " << WSAGetLastError() << std::endl;
            break;
        }

        if (FD_ISSET(socket_fd, &readfds)) {
            SOCKET client_socket = accept(socket_fd, NULL, NULL);
            if (client_socket == INVALID_SOCKET) {
                std::cerr << "Error accepting client: " << WSAGetLastError() << std::endl;
            } else {
                ioctlsocket(client_socket, FIONBIO, &mode);
                std::cout << "New client connected" << std::endl;
                client_sockets.push_back(client_socket);
            }
        }

        for (int i = 0; i < (int)client_sockets.size(); ++i) {
            SOCKET s = client_sockets[i];
            if (FD_ISSET(s, &readfds)) {
                char buf[1024];
                int bytes_received = recv(s, buf, sizeof(buf), 0);

                if (bytes_received == SOCKET_ERROR) {
                    int err_code = WSAGetLastError();
                    if (err_code == WSAEWOULDBLOCK) {
                        continue;
                    }
                    std::cerr << "Recv error: " << err_code << std::endl;
                    closesocket(s);
                    client_sockets.erase(client_sockets.begin() + i);
                    --i;
                    continue;
                }

                if (bytes_received == 0) {
                    std::cout << "Client disconnected" << std::endl;
                    closesocket(s);
                    client_sockets.erase(client_sockets.begin() + i);
                    --i;
                    continue;
                }

                std::string message(buf, bytes_received);
                std::cout << "Message received: " << message << std::endl;
                
                if (message == "exit") {
                    closesocket(s);
                    client_sockets.erase(client_sockets.begin() + i);
                    --i;
                } else {
                    send(s, message.c_str(), message.length(), 0);
                }
            }
        }
    }
}

void Server::handle_message(){
    //register
    //login
    //logout
    //exit
    //get image
}
