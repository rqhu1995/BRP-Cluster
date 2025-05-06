#pragma once

#include "Core/Population.hpp"

class Diversification {
public:
    Diversification() = default;
    
    virtual void diversify(Population& population) = 0;
}; 