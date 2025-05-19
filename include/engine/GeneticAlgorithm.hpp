#pragma once

#include "Operators/Crossover.hpp"
#include "Operators/LocalSearch.hpp"
#include "Operators/Mutation.hpp"
#include "Operators/Repair.hpp"
#include "corePopulation.hpp"
#include "coreProblemInstance.hpp"

class GeneticAlgorithm {
public:
  GeneticAlgorithm(const ProblemInstance &instance, Crossover *crossover,
                   Mutation *mutation, LocalSearch *localSearch, Repair *repair)
      : instance(instance), crossover(crossover), mutation(mutation),
        localSearch(localSearch), repair(repair) {}

  void run() {
    // TODO: Implement genetic algorithm
  }

private:
  ProblemInstance instance;
  Crossover *crossover;
  Mutation *mutation;
  LocalSearch *localSearch;
  Repair *repair;
};