// TupleClusterEvaluator.hpp

#pragma once
#include "core/station.hpp"
#include "utils/debug_utils.h"
#include <map>
#include <unordered_set>
#include <vector>

struct TransferTuple {
  std::vector<int> surplusStationIndices;
  std::vector<int> deficitStationIndices;
  std::map<std::pair<int, int>, int>
      bikeAllocations; // <which station, which station>: how many bikes
  double deltaUDF;     // Total UDF reduction from this tuple

  // print the transfer information (which station -> which station: how many
  // bikes)
  void print() const {
    for (const auto &[key, value] : bikeAllocations) {
      DEBUG_PRINT(key.first << " -> " << key.second << ": " << value);
    }
  }
};
