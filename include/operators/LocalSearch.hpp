#pragma once

#include "coreIndividual.hpp"

class LocalSearch {
public:
  LocalSearch() = default;

  virtual void improve(Individual &individual) = 0;
};