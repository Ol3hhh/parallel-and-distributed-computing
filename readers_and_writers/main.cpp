#include <atomic>
#include <iostream>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <ranges>
#include <algorithm>
#include <semaphore>
#include <csignal>
#include "types.hpp"
std::atomic<bool> is_running = true;

void signal_handler(int signum) {
    is_running = false; 
}

void setup(std::vector<Writer>& writers, std::vector<Reader>& readers, std::unique_ptr<Book>& book, std::atomic<bool>& is_running) {
    for (int i = 0; i < 3; i++) {
        readers.emplace_back(i);
        readers.back().init(*book ,is_running);
    }
    writers.emplace_back(0);
    writers.back().init(*book, is_running);

}

void circle(std::vector<Writer>& writers, std::vector<Reader>& readers, std::unique_ptr<Book>& book, std::atomic<bool>& is_running) {
    while (is_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    book->stop();
}
// TODO handle ctrl + c, reading print
int main() {
    std::signal(SIGINT, signal_handler);

    std::vector<Writer> writers;
    std::vector<Reader> readers;
    writers.reserve(1);
    readers.reserve(3);
    auto book = std::make_unique<Book>();

    setup(writers, readers, book, is_running);
    circle(writers, readers, book, is_running);

}