#pragma once

#include <iostream>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <cstring>
#include <cmath>
#include <array>
#include <charconv>
#include <immintrin.h>

class Client {
private:
    int k;
    float result;
    int clientSocket;
    sockaddr_in serverAddress;
public:
    [[nodiscard]] int setup(int& clientSocket, sockaddr_in& serverAddress);
    [[nodiscard]] int send(const int& clientSocket, const float& result);
    int receive(const int& clientSocket);
    float run();
    inline float calculate(const int &k);
};