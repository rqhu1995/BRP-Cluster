#pragma once

#include <chrono>

class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
}; 