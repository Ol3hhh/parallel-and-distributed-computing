#include "client.hpp"

int Client::setup(int& clientSocket, sockaddr_in& serverAddress) {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    while (true) {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        
        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) >= 0) {
            break; 
        }
        
        close(clientSocket);
        usleep(20000); 
    }
    return 0;
}

int Client::receive(const int& clientSocket) {
    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);
    return atoi(buffer);
}

int Client::send(const int& clientSocket, const float& result) {
    std::array<char, 32> str;
    
    auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), result);

    if (ec == std::errc()) {
        size_t length = ptr - str.data();
        ::send(clientSocket, str.data(), length, 0);
        return 0;
    }
    return 1;
}

float Client::run() {
    bool flag;
    flag = setup(clientSocket, serverAddress);
    if (flag == 1) {
        std::cerr << "Error in creating client";
        return 1;
    }

    k = receive(clientSocket);
    float result = calculate(k);
    flag = send(clientSocket, result);     

    close(clientSocket);
    return result;
}

inline float Client::calculate(const int &k) {
    
    __m256i offsets = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    
    __m256i k_base = _mm256_set1_epi32(k);
    
    __m256i k_ints = _mm256_add_epi32(k_base, offsets);

    //  Konwersja 8x int32 -> 8x float
    __m256 k_floats = _mm256_cvtepi32_ps(k_ints);

    //  Obliczenie 1.0 / k
    __m256 ones = _mm256_set1_ps(1.0f);
    __m256 v_res = _mm256_div_ps(ones, k_floats);

    //  Sumowanie poziome (Horizontal Add)
    __m128 low = _mm256_castps256_ps128(v_res);
    __m128 high = _mm256_extractf128_ps(v_res, 1);
    __m128 sum128 = _mm_add_ps(low, high);

    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);

    return _mm_cvtss_f32(sum128);
}