#include "graph.hpp"

void generate_graph(std::array<float, NN>& graph) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 100);

    float val;
    for (int i = 0; i < N; i++) {
        graph[i * N + i] = 0.0f;
        for (int j = i + 1; j < N; j++) {
            val = static_cast<float>(distr(gen));
            graph[i * N + j] = val;
            graph[j * N + i] = val;
        }
    }
}