#pragma once

#include "Core/Individual.hpp"

class LocalSearch {
public:
    LocalSearch() = default;
    
    virtual void improve(Individual& individual) = 0;
}; 