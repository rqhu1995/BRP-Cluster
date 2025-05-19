#pragma once

#include "corePopulation.hpp"

class Diversification {
public:
  Diversification() = default;

  virtual void diversify(Population &population) = 0;
};