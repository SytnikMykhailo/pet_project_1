#include "../headers/threaded_server.hpp"
#include "../headers/perlin_noise_generator.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>

ThreadedServer::ThreadedServer(int port, int addrlen, int opt, char *ip) 
    : Server(port, addrlen, opt, ip) {
}

ThreadedServer::~ThreadedServer() {
    stop();
}

void ThreadedServer::run() {
    std::cout << "Starting threaded server..." << std::endl;
    
    while (running) {
        // Очищуємо завершені потоки
        cleanup_finished_threads();
        
        // Чекаємо нових клієнтів
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms
        
        int activity = select(0, &readfds, nullptr, nullptr, &timeout);
        if (activity == SOCKET_ERROR) {
            if (running) {
                std::cerr << "Select error: " << WSAGetLastError() << std::endl;
            }
            break;
        }
        
        if (FD_ISSET(socket_fd, &readfds)) {
            SOCKET client_socket = accept(socket_fd, nullptr, nullptr);
            if (client_socket != INVALID_SOCKET) {
                // Створюємо новий потік для клієнта
                std::lock_guard<std::mutex> lock(threads_mutex);
                client_threads[client_socket] = std::thread(
                    &ThreadedServer::handle_client_thread, this, client_socket
                );
                std::cout << "New client " << client_socket << " connected. Thread created." << std::endl;
            }
        }
    }
    
    // Чекаємо завершення всіх потоків
    std::lock_guard<std::mutex> lock(threads_mutex);
    for (auto& [socket, thread] : client_threads) {
        if (thread.joinable()) {
            closesocket(socket);
            thread.join();
        }
    }
    client_threads.clear();
}

void ThreadedServer::handle_client_thread(SOCKET client_socket) {
    std::cout << "Thread started for client " << client_socket << std::endl;
    
    // Створюємо локальний стан клієнта для цього потоку
    ClientState local_state;
    
    // Встановлюємо блокуючий режим для цього клієнта
    u_long mode = 0;
    ioctlsocket(client_socket, FIONBIO, &mode);
    
    char buf[1024];
    
    try {
        while (running) {
            // Встановлюємо таймаут для recv
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, 
                      reinterpret_cast<const char*>(&timeout), sizeof(timeout));
            
            int bytes_received = recv(client_socket, buf, sizeof(buf) - 1, 0);
            
            if (bytes_received == SOCKET_ERROR) {
                int error = WSAGetLastError();
                if (error == WSAETIMEDOUT) {
                    continue; // Таймаут - продовжуємо
                }
                std::cout << "Client " << client_socket << " recv error: " << error << std::endl;
                break;
            }
            
            if (bytes_received == 0) {
                std::cout << "Client " << client_socket << " disconnected" << std::endl;
                break;
            }
            
            buf[bytes_received] = '\0';
            std::string data(buf, bytes_received);
            local_state.buffer += data;
            
            // Обробляємо всі повні повідомлення
            size_t pos;
            while ((pos = local_state.buffer.find('\n')) != std::string::npos) {
                std::string message = local_state.buffer.substr(0, pos);
                local_state.buffer.erase(0, pos + 1);
                
                if (!message.empty() && message.back() == '\r') {
                    message.pop_back();
                }
                
                std::cout << "Thread for client " << client_socket << " received: '" << message << "'" << std::endl;
                
                // ВИКОРИСТОВУЄМО local_state замість глобального
                if (handle_message_threaded(client_socket, message, local_state)) {
                    goto disconnect; // exit або помилка
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in client thread " << client_socket << ": " << e.what() << std::endl;
    }
    
disconnect:
    std::cout << "Thread for client " << client_socket << " finishing" << std::endl;
    closesocket(client_socket);
    
    // Видаляємо потік із списку
    {
        std::lock_guard<std::mutex> lock(threads_mutex);
        finished_threads.push_back(std::this_thread::get_id());
    }
}

bool ThreadedServer::handle_message_threaded(SOCKET s, const std::string& message, ClientState& state) {
    // Перевіряємо стан клієнта
    if (state.state == ClientState::WAITING_REGISTER_DATA) {
        handle_registration_data_threaded(s, message);
        state.state = ClientState::IDLE;
        return false;
    }
    else if (state.state == ClientState::WAITING_LOGIN_DATA) {
        handle_login_data_threaded(s, message);
        state.state = ClientState::IDLE;
        return false;
    }
    
    if (message == "exit") {
        const char* msg = "Goodbye!\n";
        send(s, msg, strlen(msg), 0);
        return true;
    }
    else if (message == "register") {
        const char* msg = "Send email,password,name,surname,note separated by commas:\n";
        send(s, msg, strlen(msg), 0);
        
        // Встановлюємо стан очікування даних (НЕ читаємо одразу)
        state.state = ClientState::WAITING_REGISTER_DATA;
        return false;
    }
    else if (message == "login") {
        const char* msg = "Send email,password separated by comma:\n";
        send(s, msg, strlen(msg), 0);
        
        // Встановлюємо стан очікування даних (НЕ читаємо одразу)
        state.state = ClientState::WAITING_LOGIN_DATA;
        return false;
    }
    else if (message == "logout") {
        const char* msg = "Logged out\n";
        send(s, msg, strlen(msg), 0);
        return false;
    }
    else if (message == "get image") {
        generate_and_send_image_threaded(s);
        return false;
    }
    else {
        const char* msg = "Unknown command\nAvailable commands: register, login, logout, get image, exit\n";
        send(s, msg, strlen(msg), 0);
        return false;
    }
}

void ThreadedServer::handle_registration_data_threaded(SOCKET s, const std::string& data) {
    std::cout << "Processing registration data: '" << data << "'" << std::endl; // Debug
    
    if (data.empty()) {
        const char* msg = "Registration failed - no data received\n";
        send(s, msg, strlen(msg), 0);
        return;
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
        
        std::cout << "Parsed: email='" << email << "', password='" << password 
                  << "', name='" << name << "', surname='" << surname 
                  << "', note='" << note << "'" << std::endl; // Debug
        
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
}

void ThreadedServer::handle_login_data_threaded(SOCKET s, const std::string& data) {
    if (data.empty()) {
        const char* msg = "Login failed - no data received\n";
        send(s, msg, strlen(msg), 0);
        return;
    }

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
            const char* msg = "Login failed - missing credentials\n";
            send(s, msg, strlen(msg), 0);
        }
    } else {
        const char* msg = "Login failed - invalid data format\n";
        send(s, msg, strlen(msg), 0);
    }
}

void ThreadedServer::generate_and_send_image_threaded(SOCKET s) {
    const int width = 64, height = 64;
    
    // Створюємо буфер для зображення
    std::vector<uint8_t> image_data(8 + width * height * 4);
    
    // Записуємо розміри
    uint32_t w = width, h = height;
    memcpy(image_data.data(), &w, sizeof(w));
    memcpy(image_data.data() + 4, &h, sizeof(h));
    
    // Генеруємо пікселі
    uint8_t* pixels = image_data.data() + 8;
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
    
    // Відправляємо все зображення за один раз (блокуючий режим)
    size_t sent = 0;
    while (sent < image_data.size()) {
        int chunk = send(s, reinterpret_cast<const char*>(image_data.data() + sent), 
                        image_data.size() - sent, 0);
        if (chunk <= 0) {
            std::cerr << "Error sending image data to client " << s << std::endl;
            return;
        }
        sent += chunk;
    }
    
    const char* msg = "Image sent\n";
    send(s, msg, strlen(msg), 0);
    std::cout << "Image sent to client " << s << std::endl;
}

void ThreadedServer::cleanup_finished_threads() {
    std::lock_guard<std::mutex> lock(threads_mutex);
    
    for (auto thread_id : finished_threads) {
        auto it = std::find_if(client_threads.begin(), client_threads.end(),
            [thread_id](const auto& pair) {
                return pair.second.get_id() == thread_id;
            });
        
        if (it != client_threads.end()) {
            if (it->second.joinable()) {
                it->second.join();
            }
            client_threads.erase(it);
        }
    }
    
    finished_threads.clear();
}

void ThreadedServer::stop() {
    running = false;
}