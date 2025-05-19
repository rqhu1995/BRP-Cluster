#include "clustering/tuple_evaluator.hpp"

TupleClusterEvaluator::TupleClusterEvaluator(int maxSurplus, int maxDeficit)
    : maxSurplus(maxSurplus), maxDeficit(maxDeficit) {}

ClusterEvaluationResult
TupleClusterEvaluator::evaluateCluster(const std::vector<int> &stationIndices,
                                       const std::vector<Station> &stations) {
  ClusterEvaluationResult result;
  return result;
}

void TupleClusterEvaluator::generateTuples(
    const std::vector<int> &surplusIndices,
    const std::vector<int> &deficitIndices, std::vector<TransferTuple> &tuples,
    const std::vector<Station> &stations) {
  // TODO: Implement
}

TransferTuple
TupleClusterEvaluator::evaluateTuple(const std::vector<int> &surplusIndices,
                                     const std::vector<int> &deficitIndices,
                                     const std::vector<Station> &stations) {
  // TODO: Implement
}

void TupleClusterEvaluator::filterTuples(
    const std::vector<TransferTuple> &tuples,
    const std::vector<Station> &stations) {
  // TODO: Implement
}

void TupleClusterEvaluator::greedyAllocateTuples(
    const std::vector<TransferTuple> &tuples,
    const std::vector<Station> &stations) {
  // TODO: Implement
}
