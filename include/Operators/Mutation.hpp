#pragma once

#include "Core/Individual.hpp"

class Mutation {
public:
    Mutation() = default;
    
    virtual void mutate(Individual& individual) = 0;
}; 