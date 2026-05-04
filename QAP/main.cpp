#include <iostream>
#include <array>
#include <numeric>
#include "types.hpp"
#include "graph.hpp"
#include "SA.hpp"

// Poprawiona, bezpieczna funkcja do wypisywania macierzy
void print_matrix(const std::array<float, NN>& graph) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << graph[i * N + j] << "\t";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int main() {
    // 1. Inicjalizacja generatora losowego zdefiniowanego w types.hpp
    // (Ponieważ 'gen' jest tam 'inline', jest już gotowy do użycia)

    // 2. Przypisanie początkowe: Urządzenie 'i' znajduje się w lokalizacji 'i'
    std::iota(devices.begin(), devices.end(), 0);

    // 3. Wygenerowanie symetrycznych macierzy problemu
    generate_graph(dc); // Macierz kosztów / przepływów
    generate_graph(dd); // Macierz odległości

    // Wypisujemy macierze tylko jeśli N jest na tyle małe, by zmieściły się na ekranie
    if constexpr (N <= 10) {
        std::cout << "--- Macierz przeplywow (dc) ---\n";
        print_matrix(dc);
        
        std::cout << "--- Macierz odleglosci (dd) ---\n";
        print_matrix(dd);
    }

    // 4. Ocena rozwiązania początkowego
    float initial_cost = f(devices, dc, dd);
    std::cout << "Koszt poczatkowy (przed SA): " << initial_cost << "\n";

    // 5. Uruchomienie Symulowanego Wyżarzania
    std::cout << "Rozpoczynam optymalizacje...\n";
    SA(devices, dc, dd, gen);

    // 6. Ocena i prezentacja rozwiązania końcowego
    float final_cost = f(devices, dc, dd);
    std::cout << "Koszt koncowy (po SA): " << final_cost << "\n\n";

    std::cout << "--- Najlepsze znalezione przypisanie ---\n";
    for (int i = 0; i < N; ++i) {
        std::cout << "Urzadzenie " << i << " -> Lokalizacja " << devices[i] << "\n";
    }

    return 0;
}