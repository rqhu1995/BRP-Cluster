#pragma once

#include "coreProblemInstance.hpp"
#include "coreSolution.hpp"

class HeuristicBase {
public:
  HeuristicBase() = default;

  virtual Solution solve(const ProblemInstance &instance) = 0;
};