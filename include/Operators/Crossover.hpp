#pragma once

#include "Core/Individual.hpp"

class Crossover {
public:
    Crossover() = default;
    
    virtual Individual crossover(const Individual& parent1, const Individual& parent2) = 0;
}; 