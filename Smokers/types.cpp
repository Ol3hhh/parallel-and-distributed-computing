#include "types.hpp"

// Etap żądania i używania ubijacza
void Smoker::tamp() {
    print("Requests tamper", id);
    tamper->acquire(); // Wątek zostaje uśpiony/czeka, aż zwolni się jakiś ubijacz
    
    print("Tamping...", id);
    // Symulacja czasu potrzebnego na ubicie tytoniu w fajce
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    print("Returns tamper", id);
    tamper->release(); // Zwrócenie ubijacza do puli (zwiększenie licznika semafora)
}

// Etap żądania i używania zapałek
void Smoker::light() {
    print("Requests matchbox", id);
    matchbox->acquire(); // Wątek czeka, aż zwolni się jakieś pudełko zapałek
    
    print("Lighting pipe...", id);
    // Symulacja czasu potrzebnego na odpalenie fajki
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    print("Returns matchbox", id);
    matchbox->release(); // Zwrócenie zapałek do puli
}

// etap palenia
void Smoker::smoke() {
    print("Smoking pipe...", id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    print("Finished smoking. Starting over.", id);
}

// Główna funkcja uruchamiająca cykl życia palacza
void Smoker::run() {
    job = std::thread([this]() { 
        while (true) {
            this->tamp();  // 1. Ubijanie
            this->light();  // 2. Odpalanie
            this->smoke(); // 3. Palenie
        }
    });
}

// wrapper wokół join
void Smoker::join() {
    if (job.joinable()) {
        job.join();
    }
}