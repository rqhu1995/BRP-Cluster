// TupleClusterEvaluator.hpp

#pragma once
#include "core/station.hpp"
#include <unordered_set>
#include <vector>

struct TransferTuple {
  std::vector<int> surplusStationIndices;
  std::vector<int> deficitStationIndices;
  std::vector<int> bikeAllocations; // Parallel to surplus/deficit stations, or
                                    // a matrix if you prefer
  double deltaUDF;                  // Total UDF reduction from this tuple
};
