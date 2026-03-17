#pragma once
#include <array>
#include <atomic>
#include <thread>
#include "fork.hpp"

class Philosopher {
private:
    std::atomic<int> id{0};        
    std::atomic<int> hunger{0};
    std::atomic<bool> is_alive{true};
    std::atomic<bool> eating{false};
    
    std::array<Fork*, 2> forks{nullptr, nullptr};
    std::thread worker;

    void increase_hunger();
    void check_death();

public:
    ~Philosopher();

    void set_id(int new_id);
    int get_id() const;
    int get_hunger() const;
    bool get_is_alive() const;
    bool get_eating() const;

    void assign_forks(Fork& left, Fork& right);
    void eat();
    void think();
    void init(Fork& left, Fork& right);
};