#include <iostream>
#include <cmath>
#include <sys/wait.h>
#include <unistd.h>
#include "server.hpp"
#include "client.hpp"

int main() {
    int n;
    std::cout << "Enter n: ";
    std::cin >> n;
    int p = ceil(n / 8.0);
    
    
    for (int i = 0; i < p; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            std::cerr << "Blad przy wywolywaniu fork!" << std::endl;
            return 1;
        }

        if (pid == 0) {
            Client client; 
            client.run(); 
            
            return 0; 
        }
    }

    Server server(n, p);
    float euler_gamma = server.run();
    std::cout << "Stala gamma Eulera wynosi: " << euler_gamma << std::endl;

    for (int i = 0; i < p; ++i) {
        wait(NULL);
    }

    return 0;

}