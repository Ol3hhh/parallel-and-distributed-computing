#include <atomic>
#include <iostream>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include <csignal>
#include <string>
#include <stdexcept>
#include "types.hpp"

std::atomic<bool> is_running = true;

void signal_handler(int signum) {
    std::osyncstream(std::cout) << "\n\033[1;31m[SYSTEM]\033[0m Caught signal " << signum << ". Shutting down...\n";
    is_running = false; 
}

void setup(int num_writers, int num_readers, std::vector<Writer>& writers, std::vector<Reader>& readers, std::unique_ptr<Book>& book, std::atomic<bool>& is_running) {
    for (int i = 0; i < num_readers; i++) {
        readers.emplace_back(i);
        readers.back().init(*book, is_running);
    }
    for (int i = 0; i < num_writers; i++) {
        writers.emplace_back(i);
        writers.back().init(*book, is_running);
    }
}

void circle(std::vector<Writer>& writers, std::vector<Reader>& readers, std::unique_ptr<Book>& book, std::atomic<bool>& is_running) {
    while (is_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    book->stop();
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);

    int num_writers = 1; 
    int num_readers = 3;

    try {
        if (argc >= 2) {
            num_writers = std::stoi(argv[1]);
        }
        if (argc >= 3) {
            num_readers = std::stoi(argv[2]);
        }
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << "\033[1;31m[ERROR]\033[0m Invalid arguments. Using defaults: 1 writer, 3 readers.\n";
        num_writers = 1;
        num_readers = 3;
    }

    if (num_writers <= 0 || num_readers <= 0) {
        std::osyncstream(std::cerr) << "\033[1;31m[ERROR]\033[0m Numbers must be greater than 0. Using defaults: 1 writer, 3 readers.\n";
        num_writers = 1;
        num_readers = 3;
    }

    std::osyncstream(std::cout) << "\033[1;32m[SYSTEM]\033[0m System started with " 
                                << num_writers << " writer(s) and " 
                                << num_readers << " reader(s) (Press Ctrl+C to safely terminate)\n";

    auto book = std::make_unique<Book>();

    std::vector<Writer> writers;
    std::vector<Reader> readers;
    
    writers.reserve(num_writers);
    readers.reserve(num_readers);
    
    setup(num_writers, num_readers, writers, readers, book, is_running);
    circle(writers, readers, book, is_running);

    writers.clear();
    readers.clear();

    std::osyncstream(std::cout) << "\033[1;32m[SYSTEM]\033[0m All threads finished successfully.\n";
    return 0;
}