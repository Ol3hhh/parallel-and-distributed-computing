#include "server.hpp"
#include <vector>

[[nodiscard]] int Server::setup(int& serverSocket, sockaddr_in& serverAddress) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error, can't create a socket!" << std::endl;
        return 1;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed" << std::endl;
    }

    memset(&serverAddress, 0, sizeof(serverAddress)); 
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error, can't bind!" << std::endl;
        return 1;
    }

    if (listen(serverSocket, p) < 0) {
        std::cerr << "Error, can't listen!" << std::endl;
        return 1;
    }
    
    return 0;
}

float Server::receive(int& clientSocket) { 
    char buffer[1024] = {0};
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        return std::atof(buffer); 
    }
    return 0.0f;
}

[[nodiscard]] int Server::send(const int& clientSocket, const int& k) {
    std::array<char, 10> str;
    
    auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), k);

    if (ec == std::errc()) {
        size_t length = ptr - str.data();
        ::send(clientSocket, str.data(), length, 0);
        return 0;
    }
    return 1;
}

float Server::run() {
    bool flag;
    flag = setup(serverSocket, serverAddress);

    if (flag == 1) {
        std::cerr << "Error in creating server socket" << std::endl;
        return 1;
    }

    std::vector<float> sum_k;
    sum_k.reserve(p);
    clients.reserve(p);
    
    for (int i = 0; i < p; i++) {
        clientSockets.emplace_back();
    }

    int k = 1;

    for (auto& clientSocket : clientSockets) {
        clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) continue;

        flag = send(clientSocket, k);
        k += 8; 
    }

    for (auto& clientSocket : clientSockets) {
        float partial_result = receive(clientSocket);
        sum_k.push_back(partial_result);
    }

    for (const auto& clientSocket : clientSockets) {
        close(clientSocket);
    }
    close(serverSocket);

    return euler(sum_k);
}


inline float Server::euler(std::vector<float> &sum_k) {
    auto result = std::reduce(sum_k.begin(), sum_k.end());
    
    int total_elements_computed = p * 8;
    for (int i = n + 1; i <= total_elements_computed; ++i) {
        result -= (1.0f / i);
    }

    result = result - log(n);
    return result;
}