#pragma once
#include "core/station.hpp"
#include "core/transfer_tuple.hpp"
#include <vector>

struct ClusterEvaluationResult {
  std::vector<TransferTuple> assignedTuples;
  double totalDeltaUDF;
};

class TupleClusterEvaluator {
public:
  // Patterns: e.g., maxSurplus = 3, maxDeficit = 3 for 3-to-3, etc.
  TupleClusterEvaluator(int maxSurplus, int maxDeficit);
  ClusterEvaluationResult
  evaluateCluster(const std::vector<int> &stationIndices,
                  const std::vector<Station> &stations);

private:
  int maxSurplus, maxDeficit;
  // Helper functions
  void generateTuples(const std::vector<int> &surplusIndices,
                      const std::vector<int> &deficitIndices,
                      std::vector<TransferTuple> &tuples,
                      const std::vector<Station> &stations);
  TransferTuple evaluateTuple(const std::vector<int> &surplusIndices,
                              const std::vector<int> &deficitIndices,
                              const std::vector<Station> &stations);
  void filterTuples(const std::vector<TransferTuple> &tuples,
                    const std::vector<Station> &stations);
  void greedyAllocateTuples(const std::vector<TransferTuple> &tuples,
                            const std::vector<Station> &stations);
};
