#pragma once

#include <random>

class Random {
public:
    static Random& getInstance() {
        static Random instance;
        return instance;
    }
    
    int getInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator);
    }
    
    double getDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator);
    }
    
private:
    Random() : generator(std::random_device{}()) {}
    std::mt19937 generator;
}; 