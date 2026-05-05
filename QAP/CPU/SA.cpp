#include "SA.hpp"
#include "types.hpp"
#include "utils.hpp"


bool P(float& E, float& E_next, float& T, std::mt19937& gen) {
    if (E_next < E) {
        return true;
    }
 
    float propability = exp(-(E_next-E)/T);

    std::uniform_real_distribution<float> dstr(0.0f, 1.0);
    return dstr(gen) < propability;
}

std::pair<int, int> pi(std::mt19937& gen) {
    std::uniform_int_distribution<> distr(0, N-1);

    int idx1 = static_cast<int>(distr(gen));
    int idx2 = static_cast<int>(distr(gen));
    
    while (idx1 == idx2) {
        idx2 = distr(gen);
    }

    return {idx1, idx2};
}

void update_T(float &T) {
    T = T*a;
}

std::tuple<float, int, int> calc_cost(std::array<int, N>& devices, const std::array<float, NN>& dc, const std::array<float, NN>& dd, float prev_cost, std::mt19937& gen) {
    auto [idx1, idx2] = pi(gen);
    float delta = 0;
    
    for (int k = 0; k < N; k++) {
        if (k == idx1 || k == idx2) continue;

        float old_rel = dc[idx1*N + k] * dd[devices[idx1]*N + devices[k]] +
                        dc[idx2*N + k] * dd[devices[idx2]*N + devices[k]];
        
        float new_rel = dc[idx1*N + k] * dd[devices[idx2]*N + devices[k]] +
                        dc[idx2*N + k] * dd[devices[idx1]*N + devices[k]];
        
        delta += (new_rel - old_rel);
    }

    std::swap(devices[idx1], devices[idx2]);
    return std::make_tuple(prev_cost + delta * 2, idx1, idx2);
}

void SA(std::array<int, N>& devices, const std::array<float, NN>& dc, const std::array<float, NN>& dd, std::mt19937& gen) {

    float E_prev = f(devices, dc, dd);
    float local_T = T;
    while (local_T > T_min) {
        auto [E, idx1, idx2] = calc_cost(devices, dc, dd, E_prev, gen);
        if (P(E_prev, E, local_T, gen)) {
            E_prev = E;

        }
        else {
                std::swap(devices[idx2], devices[idx1]);

        }
        update_T(local_T);

    }
}