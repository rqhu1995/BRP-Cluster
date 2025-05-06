#pragma once

#include <vector>
#include "Core/Individual.hpp"

class Population {
public:
    Population() = default;
    
    void addIndividual(const Individual& individual) {
        individuals.push_back(individual);
    }
    
    const std::vector<Individual>& getIndividuals() const {
        return individuals;
    }
    
private:
    std::vector<Individual> individuals;
}; 