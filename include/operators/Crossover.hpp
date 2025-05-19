#pragma once

#include "coreIndividual.hpp"

class Crossover {
public:
  Crossover() = default;

  virtual Individual crossover(const Individual &parent1,
                               const Individual &parent2) = 0;
};