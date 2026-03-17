#include "philosopher.hpp"
#include <chrono>

void Philosopher::increase_hunger() {
    if (hunger < 10) hunger++;
}

void Philosopher::check_death() {
    if (hunger >= 10 && is_alive) {
        is_alive = false;
    }
}

Philosopher::~Philosopher() {
    is_alive = false; 
    if (worker.joinable()) {
        worker.join();
    }
}

void Philosopher::set_id(int new_id) { id = new_id; }
int Philosopher::get_id() const { return id.load(); }
int Philosopher::get_hunger() const { return hunger.load(); }
bool Philosopher::get_is_alive() const { return is_alive.load(); }
bool Philosopher::get_eating() const { return eating.load(); }

void Philosopher::assign_forks(Fork& left, Fork& right) {
    forks[0] = &left;
    forks[1] = &right;
}

void Philosopher::eat() {
    if (!is_alive) return;

    bool got_forks = false;
    while (is_alive && !got_forks) {
        std::unique_lock<std::mutex> lock1(forks[0]->mtx, std::defer_lock);
        std::unique_lock<std::mutex> lock2(forks[1]->mtx, std::defer_lock);
        
        if (std::try_lock(lock1, lock2) == -1) {
            got_forks = true;
            eating = true;
            forks[0]->owner_id = id.load();
            forks[1]->owner_id = id.load();

            hunger = 0; 
            std::this_thread::sleep_for(std::chrono::milliseconds(600)); 

            forks[0]->owner_id = -1;
            forks[1]->owner_id = -1;
            eating = false;
        } else {
            increase_hunger();
            check_death();
            std::this_thread::sleep_for(std::chrono::milliseconds(400)); 
        }
    }
}

void Philosopher::think() {
    if (!is_alive) return;
    increase_hunger();
    check_death();
    std::this_thread::sleep_for(std::chrono::milliseconds(600)); 
}

void Philosopher::init(Fork& left, Fork& right) {
    assign_forks(left, right);
    worker = std::thread([this](){
        while (is_alive) {
            if (hunger < 3) {
                think();
            } else {
                eat();
            }
        }
    });
}