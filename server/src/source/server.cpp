#include "../headers/server.hpp"
#include "../headers/hash_table.hpp"
#include <filesystem>
#include <string>
#include <sstream>
#include "../headers/perlin_noise_generator.hpp"
#include <algorithm>
#include <map>
#include <iostream>
#include <cstdint>
#include <cstring>     // Додаємо для memcpy


std::map<SOCKET, Server::ClientState> client_states;

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
    fd_set readfds, writefds;

    while (true) {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(socket_fd, &readfds);
        SOCKET max_sd = socket_fd;

        for (SOCKET s : client_sockets) {
            FD_SET(s, &readfds);
            
            // Додаємо в writefds якщо потрібно відправити дані
            auto it = client_states.find(s);
            if (it != client_states.end() && 
                (it->second.state == ClientState::SENDING_IMAGE_HEADER || 
                 it->second.state == ClientState::SENDING_IMAGE_DATA)) {
                FD_SET(s, &writefds);
            }
            
            if (s > max_sd) {
                max_sd = s;
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms timeout

        int activity = select(0, &readfds, &writefds, NULL, &timeout);
        if (activity == SOCKET_ERROR) {
            std::cerr << "Select error: " << WSAGetLastError() << std::endl;
            break;
        }

        // Обробка нових підключень
        if (FD_ISSET(socket_fd, &readfds)) {
            SOCKET client_socket = accept(socket_fd, NULL, NULL);
            if (client_socket != INVALID_SOCKET) {
                ioctlsocket(client_socket, FIONBIO, &mode);
                std::cout << "New client connected: " << client_socket << std::endl;
                client_sockets.push_back(client_socket);
                client_states[client_socket] = ClientState();
            }
        }

        // Обробка даних від клієнтів
        for (int i = 0; i < (int)client_sockets.size(); ++i) {
            SOCKET s = client_sockets[i];
            
            // Читання даних
            if (FD_ISSET(s, &readfds)) {
                if (handle_client_read(s)) {
                    std::cout << "Client " << s << " disconnected" << std::endl;
                    closesocket(s);
                    client_states.erase(s);
                    client_sockets.erase(client_sockets.begin() + i);
                    --i;
                    continue;
                }
            }
            
            // Відправка даних
            if (FD_ISSET(s, &writefds)) {
                handle_client_write(s);
            }
        }
    }
}

bool Server::handle_client_read(SOCKET s) {
    char buf[1024];
    int bytes_received = recv(s, buf, sizeof(buf) - 1, 0);

    if (bytes_received == SOCKET_ERROR) {
        int err_code = WSAGetLastError();
        if (err_code != WSAEWOULDBLOCK) {
            std::cerr << "Recv error: " << err_code << std::endl;
            return true; // Відключити клієнта
        }
        return false;
    }

    if (bytes_received == 0) {
        return true; // Клієнт відключився
    }

    buf[bytes_received] = '\0';
    std::string data(buf, bytes_received);
    
    ClientState& state = client_states[s];
    
    // Додаємо дані до буфера
    state.buffer += data;
    
    // Обробляємо повні повідомлення (розділені \n)
    size_t pos;
    while ((pos = state.buffer.find('\n')) != std::string::npos) {
        std::string message = state.buffer.substr(0, pos);
        state.buffer.erase(0, pos + 1);
        
        // Видаляємо \r якщо є
        if (!message.empty() && message.back() == '\r') {
            message.pop_back();
        }
        
        std::cout << "Message from " << s << ": '" << message << "'" << std::endl;
        
        if (handle_message_stateful(s, message)) {
            return true; // Відключити клієнта
        }
    }
    
    return false;
}

void Server::handle_client_write(SOCKET s) {
    ClientState& state = client_states[s];
    
    if (state.state == ClientState::SENDING_IMAGE_HEADER) {
        // Відправляємо заголовок зображення (розміри)
        const char* header_data = reinterpret_cast<const char*>(state.image_data.data()) + state.bytes_sent;
        size_t remaining = 8 - state.bytes_sent; // 4 bytes width + 4 bytes height
        
        int sent = send(s, header_data, remaining, 0);
        if (sent > 0) {
            state.bytes_sent += sent;
            if (state.bytes_sent >= 8) {
                state.state = ClientState::SENDING_IMAGE_DATA;
                state.bytes_sent = 8; // Починаємо з пікселів
            }
        }
    }
    else if (state.state == ClientState::SENDING_IMAGE_DATA) {
        // Відправляємо піксельні дані
        const char* pixel_data = reinterpret_cast<const char*>(state.image_data.data()) + state.bytes_sent;
        size_t remaining = state.image_data.size() - state.bytes_sent;
        
        int sent = send(s, pixel_data, std::min(remaining, size_t(1024)), 0);
        if (sent > 0) {
            state.bytes_sent += sent;
            if (state.bytes_sent >= state.image_data.size()) {
                const char* msg = "Image sent\n";
                send(s, msg, strlen(msg), 0);
                state.state = ClientState::IDLE;
                state.bytes_sent = 0;
                state.image_data.clear();
            }
        }
    }
}

bool Server::handle_message_stateful(SOCKET s, const std::string& message) {
    ClientState& state = client_states[s];
    
    switch (state.state) {
        case ClientState::IDLE:
            return handle_command(s, message, state);
        case ClientState::WAITING_REGISTER_DATA:
            return handle_registration_data(s, message, state);
        case ClientState::WAITING_LOGIN_DATA:
            return handle_login_data(s, message, state);
        default:
            return false;
    }
}

bool Server::handle_command(SOCKET s, const std::string& message, ClientState& state) {
    if (message == "exit") {
        const char* msg = "Goodbye!\n";
        send(s, msg, strlen(msg), 0);
        return true; // Відключити клієнта
    }
    else if (message == "register") {
        const char* msg = "Send email,password,name,surname,note separated by commas:\n";
        send(s, msg, strlen(msg), 0);
        state.state = ClientState::WAITING_REGISTER_DATA;
        return false;
    }
    else if (message == "login") {
        const char* msg = "Send email,password separated by comma:\n";
        send(s, msg, strlen(msg), 0);
        state.state = ClientState::WAITING_LOGIN_DATA;
        return false;
    }
    else if (message == "logout") {
        const char* msg = "Logged out\n";
        send(s, msg, strlen(msg), 0);
        return false;
    }
    else if (message == "get image") {
        generate_and_queue_image(s, state);
        return false;
    }
    else {
        const char* msg = "Unknown command\nAvailable commands: register, login, logout, get image, exit\n";
        send(s, msg, strlen(msg), 0);
        return false;
    }
}

bool Server::handle_registration_data(SOCKET s, const std::string& data, ClientState& state) {
    if (data.empty()) {
        const char* msg = "Registration failed - no data received\n";
        send(s, msg, strlen(msg), 0);
        state.state = ClientState::IDLE;
        return false;
    }

    std::istringstream iss(data);
    std::string email, password, name, surname, note;
    
    if (std::getline(iss, email, ',') &&
        std::getline(iss, password, ',') &&
        std::getline(iss, name, ',') &&
        std::getline(iss, surname, ',') &&
        std::getline(iss, note)) {
        
        // Видаляємо зайві пробіли
        email.erase(email.find_last_not_of(" \n\r\t") + 1);
        password.erase(password.find_last_not_of(" \n\r\t") + 1);
        name.erase(name.find_last_not_of(" \n\r\t") + 1);
        surname.erase(surname.find_last_not_of(" \n\r\t") + 1);
        note.erase(note.find_last_not_of(" \n\r\t") + 1);
        
        if (!email.empty() && !password.empty() && !name.empty() && !surname.empty()) {
            try {
                db->insert_user(email.c_str(), password.c_str(), name.c_str(), surname.c_str(), note.c_str());
                const char* msg = "Registration successful\n";
                send(s, msg, strlen(msg), 0);
            } catch (const std::exception& e) {
                std::cerr << "Database error: " << e.what() << std::endl;
                const char* msg = "Registration failed - database error\n";
                send(s, msg, strlen(msg), 0);
            }
        } else {
            const char* msg = "Registration failed - missing required fields\n";
            send(s, msg, strlen(msg), 0);
        }
    } else {
        const char* msg = "Registration failed - invalid data format\n";
        send(s, msg, strlen(msg), 0);
    }
    
    state.state = ClientState::IDLE;
    return false;
}

bool Server::handle_login_data(SOCKET s, const std::string& data, ClientState& state) {
    std::istringstream iss(data);
    std::string email, password;
    
    if (std::getline(iss, email, ',') && std::getline(iss, password)) {
        email.erase(email.find_last_not_of(" \n\r\t") + 1);
        password.erase(password.find_last_not_of(" \n\r\t") + 1);
        
        if (!email.empty() && !password.empty()) {
            try {
                int user_id = db->find_user(email.c_str());
                if (user_id != -1) {
                    const char* msg = "Login successful\n";
                    send(s, msg, strlen(msg), 0);
                } else {
                    const char* msg = "Login failed - user not found\n";
                    send(s, msg, strlen(msg), 0);
                }
            } catch (const std::exception& e) {
                std::cerr << "Database error: " << e.what() << std::endl;
                const char* msg = "Login failed - database error\n";
                send(s, msg, strlen(msg), 0);
            }
        } else {
            const char* msg = "Login failed - missing email or password\n";
            send(s, msg, strlen(msg), 0);
        }
    } else {
        const char* msg = "Login failed - invalid data format\n";
        send(s, msg, strlen(msg), 0);
    }
    
    state.state = ClientState::IDLE;
    return false;
}

void Server::generate_and_queue_image(SOCKET s, ClientState& state) {
    const int width = 64, height = 64;
    
    // Підготовка буфера: 8 байт заголовок + піксельні дані
    state.image_data.resize(8 + width * height * 4);
    
    // Записуємо розміри у заголовок
    uint32_t w = width, h = height;
    memcpy(state.image_data.data(), &w, sizeof(w));
    memcpy(state.image_data.data() + 4, &h, sizeof(h));
    
    // Генеруємо піксельні дані
    uint8_t* pixels = state.image_data.data() + 8;
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            double noise = PerlinNoise2D::fractalBrownianMotion(row * 0.9, col * 0.9, 8, 1.0);
            noise = (noise + 1.0) / 2.0;
            uint8_t r = 0, g = 0, b = 0, a = 255;
            if (noise < 0.3)      { r = 255; }
            else if (noise < 0.7) { g = 255; }
            else                  { b = 255; }
            int idx = (row * width + col) * 4;
            pixels[idx + 0] = r;
            pixels[idx + 1] = g;
            pixels[idx + 2] = b;
            pixels[idx + 3] = a;
        }
    }
    
    state.state = ClientState::SENDING_IMAGE_HEADER;
    state.bytes_sent = 0;
    
    // Використовуємо параметр s, щоб уникнути warning
    std::cout << "Generated image for client " << s << std::endl;
}