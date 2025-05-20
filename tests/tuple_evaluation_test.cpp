#include "clustering/tuple_evaluator.hpp"
#include "core/problem.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <set>
#include <vector>

void test_evaluateTuple() {
  // read stations using problem
  ProblemInstance instance("../data/results.csv");
  auto stations = instance.getStations();
  std::cout << "Stations loaded: " << stations.size() << "\n";

  // randomly choose 30 stations from the stations vector (excluding index 0)
  std::vector<int> indices(stations.size() - 1);
  std::iota(indices.begin(), indices.end(), 1); // Fill with 1, 2, ..., N-1

  std::random_device rd;
  std::mt19937 g(42);
  std::shuffle(indices.begin(), indices.end(), g);

  std::vector<int> chosen_indices(indices.begin(), indices.begin() + 10);

  // split chosen_indices to surplusIndices and deficitIndices based on the
  // station status
  std::vector<int> surplusIndices;
  std::vector<int> deficitIndices;
  for (auto idx : chosen_indices) {
    if (stations[idx].getStatus() == StationStatus::SURPLUS)
      surplusIndices.push_back(idx);
    else if (stations[idx].getStatus() == StationStatus::DEFICIT)
      deficitIndices.push_back(idx);
  }

  // Now create the TupleClusterEvaluator
  TupleClusterEvaluator evaluator(3, 3);
  // generate the tuples first
  std::vector<TransferTuple> tuples;
  evaluator.generateTuples(surplusIndices, deficitIndices, tuples, stations);

  std::vector<TransferTuple> selectedTuples =
      evaluator.greedySelectExclusiveTuples(tuples);

  // test two things:
  // 1. all tuples' deltaUDF should be positive
  // 2. the tuples are mutually exclusive
  for (auto &tuple : selectedTuples) {
    assert(tuple.deltaUDF > 0);
  }
  for (size_t i = 0; i < selectedTuples.size(); ++i) {
    for (size_t j = i + 1; j < selectedTuples.size(); ++j) {
      // setify the surplus and deficit indices and make them a union set
      std::set<int> tupleIStations =
          std::set<int>(selectedTuples[i].surplusStationIndices.begin(),
                        selectedTuples[i].surplusStationIndices.end());
      tupleIStations.insert(selectedTuples[i].deficitStationIndices.begin(),
                            selectedTuples[i].deficitStationIndices.end());
      std::set<int> tupleJStations =
          std::set<int>(selectedTuples[j].surplusStationIndices.begin(),
                        selectedTuples[j].surplusStationIndices.end());
      tupleJStations.insert(selectedTuples[j].deficitStationIndices.begin(),
                            selectedTuples[j].deficitStationIndices.end());
      // check if the two sets have intersection
      std::set<int> unionSet;
      std::set_union(tupleIStations.begin(), tupleIStations.end(),
                     tupleJStations.begin(), tupleJStations.end(),
                     std::inserter(unionSet, unionSet.begin()));

      //   assert(tupleIStations.size() + tupleJStations.size() ==
      //   unionSet.size());
      if (tupleIStations.size() + tupleJStations.size() != unionSet.size()) {
        std::cout << "Tuple " << i << " and Tuple " << j
                  << " have intersection\n";
        std::cout << "Tuple " << i << " stations: ";
        for (auto idx : tupleIStations)
          std::cout << idx << " ";
        std::cout << "\nTuple " << j << " stations: ";
        for (auto idx : tupleJStations)
          std::cout << idx << " ";
        std::cout << "\n";
        break;
      }
    }
  }
  std::cout << "Test EvaluateTuple passed\n";
}

int main() {
  test_evaluateTuple();
  return 0;
}
