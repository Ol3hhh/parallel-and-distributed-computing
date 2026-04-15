#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <string>
#include "types.hpp"

int main(int argc, char *argv[]) {
    int k = 3; //  palaczy
    int l = 2; //  ubijaczy
    int m = 1; //  pudełek zapałek

    if (argc == 4) {
        k = std::stoi(argv[1]); 
        l = std::stoi(argv[2]);
        m = std::stoi(argv[3]);
    }

    // Inicjalizacja puli współdzielonych zasobów za pomocą semaforów liczących
    Tamper tamper_pool(l);
    Matchbox matchbox_pool(m);

    // Wektor przechowujący unikalne wskaźniki na obiekty palaczy
    std::vector<std::unique_ptr<Smoker>> smokers;
    smokers.reserve(k);

    // Tworzenie i uruchamianie wątków dla każdego palacza
    for (int i = 0; i < k; i++) {
        smokers.push_back(std::make_unique<Smoker>(i + 1, &tamper_pool, &matchbox_pool));
        smokers.back()->run(); 
    }
    
    for (auto& smoker : smokers) {
        smoker->join();
    }
    
    return 0;
}