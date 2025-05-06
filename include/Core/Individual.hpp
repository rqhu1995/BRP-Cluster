#pragma once

#include "Core/Solution.hpp"

class Individual {
public:
    Individual() = default;
    
    const Solution& getSolution() const { return solution; }
    void setSolution(const Solution& sol) { solution = sol; }
    
    double getFitness() const { return fitness; }
    void setFitness(double f) { fitness = f; }
    
private:
    Solution solution;
    double fitness = 0.0;
}; 