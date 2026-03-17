#pragma once
#include <mutex>

struct Fork {
    int id;
    int owner_id = -1;
    std::mutex mtx;
};