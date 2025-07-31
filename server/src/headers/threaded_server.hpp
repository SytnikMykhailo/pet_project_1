#pragma once
#include "server.hpp"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <vector>

class ThreadedServer : public Server {
public:
    ThreadedServer(int port, int addrlen, int opt, char *ip);
    ~ThreadedServer() override;
    
    void run() override;
    void stop();

private:
    void generate_and_send_image_threaded(SOCKET s);
    void handle_client_thread(SOCKET client_socket);
    void cleanup_finished_threads();
    bool handle_message_threaded(SOCKET s, const std::string& message, ClientState& state);
    void handle_registration_data_threaded(SOCKET s, const std::string& data);
    void handle_login_data_threaded(SOCKET s, const std::string& data);
    
    std::atomic<bool> running{true};
    std::mutex threads_mutex;
    std::unordered_map<SOCKET, std::thread> client_threads;
    std::vector<std::thread::id> finished_threads;
};