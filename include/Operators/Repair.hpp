#pragma once

#include "Core/Individual.hpp"

class Repair {
public:
    Repair() = default;
    
    virtual void repair(Individual& individual) = 0;
}; 