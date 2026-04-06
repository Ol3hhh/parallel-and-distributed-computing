#pragma once
#include <iostream>
#include <syncstream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>
#include <random>
#include <functional> 

class Book {
private:
    mutable std::shared_mutex m;
    mutable std::condition_variable_any cv;
    int num = 0;
    mutable std::atomic<int> read_count{0};
    std::atomic<bool> first_write_done{false};
    bool stopped{false};
public:
    template <typename Func>
    void set_num(int new_num, Func&& print_action) {
        std::unique_lock<std::shared_mutex> lock(m);
        cv.wait(lock, [this]() { return stopped || !first_write_done.load() || read_count.load() >= 3; });
        if (stopped) return;
        
        this->num = new_num;
        read_count = 0;
        first_write_done = true;
        
        print_action(); 
        
        cv.notify_all();
    }

    template <typename Func>
    int get_num(Func&& print_action) const {
        std::shared_lock<std::shared_mutex> lock(m);
        cv.wait(lock, [this]() { return stopped || first_write_done.load(); });
        if (stopped) return num;
        
        
        print_action(num);
        
        if (++read_count == 3) {
            cv.notify_all(); 
        }
        return num;   
    }
    
    void stop() {
        std::unique_lock<std::shared_mutex> lock(m);
        stopped = true;
        cv.notify_all();
    }
};

class Reader {
private:
    int id;
    std::thread worker;
public:
    Reader(int _id) : id(_id) {}
    Reader(Reader&&) = default;
    Reader(const Reader&) = delete;
    ~Reader() {
        if (worker.joinable()) {
            worker.join();
        }
    }
    void init(const Book& book, std::atomic<bool>& is_running) {
        worker = std::thread([this, &book, &is_running]() { 
            while (is_running) {
                this->read(book); 
            }
        });
    }

    void read(const Book& book) const {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        book.get_num([this](int read_num) {
            std::osyncstream(std::cout) << "\033[1;36m[READER " << id << "]\033[0m Read number: " << read_num << "\n";
        });
    }
};

class Writer {
private:
    int id;
    std::thread worker;
public:
    Writer(int _id) : id(_id) {}
    Writer(Writer&&) = default;
    Writer(const Writer&) = delete;
    ~Writer() {
        if (worker.joinable()) {
            worker.join();
        }
    }
    void init(Book& book, std::atomic<bool>& is_running) {
        worker = std::thread([this, &book, &is_running]() { 
            while(is_running) {
                this->write(book, get_random()); 
            }
        });
    }

    void write(Book& book, int num) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        book.set_num(num, [this, num]() {
            std::osyncstream(std::cout) << "\033[1;33m[WRITER " << id << "]\033[0m Wrote number: " << num << "\n";
        });
    }

    static int get_random() {
        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(1, 100);
        return distr(gen);
    }
};