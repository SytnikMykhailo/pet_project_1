#include "../headers/server.hpp"

Server *server = nullptr;

void handle_signal(int signum) {
    std::cout << "Signal: " << signum << std::endl;
    closesocket(server->socket_fd);
    WSACleanup();
    free(server);
    exit(EXIT_SUCCESS);
}

Server::~Server(){
    WSACleanup();
    closesocket(socket_fd);
}

Server::Server(int port, int addrlen, int opt, char *ip){
    this->port = port;
    this->addrlen = addrlen;
    this->opt = opt;
    this->server_address.sin_family = AF_INET;
    this->server_address.sin_port = htons(port);
    this->server_address.sin_addr.s_addr = inet_addr(ip);
    this->db = Database("users.db");
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
    signal(SIGTERM, handle_signal);
}

void Server::run(){
    std::vector<SOCKET> client_sockets; 
    fd_set readfds;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);

        if(FD_ISSET(socket_fd, &readfds)){
            SOCKET client_socket = accept(socket_fd, NULL, NULL);
            if(client_socket == INVALID_SOCKET){
                std::cerr << "Error accepting client" << std::endl;
            }else{
                ioctlsocket(client_socket, FIONBIO, &mode);
                std::cout << "New client added" << std::endl;
                client_sockets.push_back(client_socket);
            }
        }
        for(SOCKET socket : client_sockets){
            FD_SET(socket, &readfds);
        }
        if(select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR){
            WSACleanup();
            std::cerr << "Select error" << std::endl;
            return;
        }
        std::cout << "Entering loop" << std::endl;
        for(int i = 0; i < (int)client_sockets.size(); i++){
            if(FD_ISSET(client_sockets.at(i), &readfds)){
                char buf[1024];
                int bytes_recieved = recv(client_sockets.at(i), buf, sizeof(buf), 0);
                if(bytes_recieved == -1){
                    std::cerr << "Recv error" << std::endl;
                    closesocket(client_sockets.at(i));
                    client_sockets.erase(client_sockets.begin() + i);
                    i--;
                }else if(bytes_recieved == 0){
                    std::cout << "Client disconected" << std::endl;
                    client_sockets.erase(client_sockets.begin() + i);
                    i--;
                }else{
                    std::string message(buf, bytes_recieved);
                    std::cout << "Message recieved: " << message << std::endl;
                    if(message == "exit"){
                        closesocket(client_sockets.at(i));
                        client_sockets.erase(client_sockets.begin() + i);
                        i--;
                    }else{
                        send(client_sockets.at(i), message.c_str(), message.size(), 0);
                    }
                }
            }
        }
    }
}

/*int main(int argc, char **argv) {
    if(argc != 3){
        std::cerr << "Wrong number of arguments" << std::endl;
        return -1;
    }
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
    free(server);
    return 0;
}*/