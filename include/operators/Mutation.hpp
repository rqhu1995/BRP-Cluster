#pragma once

#include "coreIndividual.hpp"

class Mutation {
public:
  Mutation() = default;

  virtual void mutate(Individual &individual) = 0;
};