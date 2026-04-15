#pragma once

#include <iostream>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <cstring>
#include <cmath>
#include <vector>
#include <array>
#include <charconv>
#include <numeric>       
#include "client.hpp"

class Server {
private:
    int n;
    int p;

    int serverSocket;
    sockaddr_in serverAddress;

    std::vector<int> clientSockets;
    std::vector<Client> clients;
public:
    Server(int n_, int p_) : n(n_), p(p_) {}
    Server() {
        clientSockets.reserve(n);
    }

    [[nodiscard]] int setup(int& serverSocket, sockaddr_in& serverAddress);
    [[nodiscard]] int send(const int& clientSocket, const int& k);
    float receive(int& clientSocket);
    float run();
    inline float euler(std::vector<float> &sum_k);
};