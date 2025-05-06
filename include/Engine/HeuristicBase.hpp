#pragma once

#include "Core/Solution.hpp"
#include "Core/ProblemInstance.hpp"

class HeuristicBase {
public:
    HeuristicBase() = default;
    
    virtual Solution solve(const ProblemInstance& instance) = 0;
}; 