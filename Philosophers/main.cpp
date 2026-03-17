#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <memory>
#include <string>
#include <stdexcept>
#include <ncurses.h> 
#include "philosopher.hpp"

void setup(std::vector<std::unique_ptr<Philosopher>>& philosophers, 
           std::vector<std::unique_ptr<Fork>>& forks, int N) {
    for (int i = 0; i < N; i++) {
        philosophers.push_back(std::make_unique<Philosopher>());
        philosophers.back()->set_id(i);
        
        forks.push_back(std::make_unique<Fork>());
        forks.back()->id = i;
    }

    for (int i = 0; i < N; i++) {
        philosophers[i]->init(*forks[i], *forks[(i + 1) % N]);
    }
}

void dining(const std::vector<std::unique_ptr<Philosopher>>& philosophers) {    
    bool running = true;
    int N = philosophers.size();
    
    while (running) {
        erase(); 
        mvprintw(0, 0, "=== DINING PHILOSOPHERS PROBLEM ===");
        mvprintw(1, 0, "[Press \"q\" to exit]");
        
        int dead_count = 0;
        for (int i = 0; i < N; i++) {
            int y = i + 3;
            mvprintw(y, 0, "Philosopher %d:", philosophers[i]->get_id());

            if (!philosophers[i]->get_is_alive()) {
                mvprintw(y, 14, "[ DEAD ]");
                dead_count++;
            } else if (philosophers[i]->get_eating()) {
                mvprintw(y, 14, "[  EAT   ]");
            } else {
                mvprintw(y, 14, "[ THINK  ]");
            }

            mvprintw(y, 28, "Hunger: %d/10", philosophers[i]->get_hunger());
        }
        
        if (dead_count == N) {
            mvprintw(N + 4, 0, "All philosophers are dead. Press 'q' to exit.");
        }
        
        refresh(); 

        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(int argc, char* argv[]) {
    int N = 5;

    if (argc == 2) {
        try {
            N = std::stoi(argv[1]);
            if (N < 2) {
                std::cerr << "Error: Provided number must be at least 2." << std::endl;
                return 1;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: provided unproper number." << std::endl;
            return 1;
        }
    } else if (argc > 2) {
        std::cerr << "Use of: " << argv[0] << " [Philosophers]" << std::endl;
        return 1;
    }

    initscr();              
    noecho();              
    cbreak();               
    curs_set(0);           
    nodelay(stdscr, TRUE); 

    std::vector<std::unique_ptr<Philosopher>> philosophers;
    std::vector<std::unique_ptr<Fork>> forks;

    philosophers.reserve(N);
    forks.reserve(N);

    setup(philosophers, forks, N);
    dining(philosophers);

    endwin();

    std::cout << "Simulation ended" << std::endl;
    return 0;
}