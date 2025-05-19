#pragma once

#include "coreIndividual.hpp"

class Repair {
public:
  Repair() = default;

  virtual void repair(Individual &individual) = 0;
};