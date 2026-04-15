#pragma once
#include <thread>
#include <semaphore>
#include <string>
#include <chrono>
#include <iostream>

// Klasa reprezentująca dostępną pulę ubijaczy
class Tamper {
private:
    // Semafor liczący pilnujący dostępu do 'l' ubijaczy
    std::counting_semaphore<> sem;
public:
    Tamper(int k) : sem(k) {}
    
    // Metody do zajmowania (zmniejszanie semafora) i zwalniania (zwiększanie) zasobu
    void acquire() { sem.acquire(); }
    void release() { sem.release(); }
};

// Klasa reprezentująca dostępną pulę pudełek zapałek
class Matchbox {
private:
    // Semafor liczący pilnujący dostępu do 'm' pudełek zapałek
    std::counting_semaphore<> sem;
public:
    Matchbox(int k) : sem(k) {}

    void acquire() { sem.acquire(); }
    void release() { sem.release(); }
};

// Klasa reprezentująca pojedynczego palacza
class Smoker {
private:
    std::thread job; // Obiekt wątku, w którym będzie działać pętla danego palacza

    // Statyczny semafor binarny (o wartości 1) do wzajemnego wykluczania przy wypisywaniu na ekran.
    // Gwarantuje, że komunikaty z różnych wątków nie nałożą się na siebie w konsoli.
    static inline std::counting_semaphore<1> print_sem{1}; 

    // Wskaźniki na współdzielone pule zasobów (wszyscy palacze korzystają z tych samych instancji)
    Tamper* tamper;
    Matchbox* matchbox;

    int id; // Unikalny identyfikator palacza

    // Prywatne metody odpowiadające za konkretne akcje
    void tamp();
    void light();

public:
    // Konstruktor inicjalizujący stan początkowy palacza
    Smoker(int id_, Tamper* tamper_, Matchbox* matchbox_) 
        : id(id_), tamper(tamper_), matchbox(matchbox_) {}
    
    // Destruktor upewniający się, że wątek został poprawnie zamknięty w przypadku zniszczenia obiektu
    ~Smoker() {
        join();
    }

    void smoke();
    void run();
    void join();

    // funkcja wypisująca komunikaty na standardowe wyjście z użyciem semafora
    static void print(const std::string& str, const int& id) {
        print_sem.acquire(); // Zablokowanie dostępu do konsoli dla innych wątków
        std::cout << "Smoker " << id << ": " << str << std::endl;
        print_sem.release(); // Odblokowanie dostępu do konsoli
    }
};