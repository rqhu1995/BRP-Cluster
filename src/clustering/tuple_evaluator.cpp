#include "clustering/tuple_evaluator.hpp"
#include "core/transfer_tuple.hpp"
#include "utils/debug_utils.h"
#include <algorithm>
#include <set>
#include <vector>

TupleClusterEvaluator::TupleClusterEvaluator(int maxSurplus, int maxDeficit)
    : maxSurplus(maxSurplus), maxDeficit(maxDeficit) {}

ClusterEvaluationResult
TupleClusterEvaluator::evaluateCluster(const std::vector<int> &stationIndices,
                                       const std::vector<Station> &stations) {
  ClusterEvaluationResult result;
  // step 0: split the stationIndices into surplus and deficit
  std::vector<int> surplusIndices;
  std::vector<int> deficitIndices;
  for (int i = 0; i < stationIndices.size(); i++) {
    if (stations[stationIndices[i]].getStatus() == StationStatus::SURPLUS) {
      surplusIndices.push_back(stationIndices[i]);
    } else if (stations[stationIndices[i]].getStatus() ==
               StationStatus::DEFICIT) {
      deficitIndices.push_back(stationIndices[i]);
    }
  }
  // step 1: decide the tuples in the cluster
  std::vector<TransferTuple> tuples;
  generateTuples(surplusIndices, deficitIndices, tuples, stations);

  // step 2: evaluate the tuple by summing up the delta UDFs
  for (auto &tuple : tuples) {
    result.totalDeltaUDF += tuple.deltaUDF;
  }
  // step 3: return the result
  return result;
}

void TupleClusterEvaluator::generateTuples(
    const std::vector<int> &surplusIndices,
    const std::vector<int> &deficitIndices, std::vector<TransferTuple> &tuples,
    const std::vector<Station> &stations) {

  std::vector<std::set<int>> acceptedSurSets, acceptedDefSets;
  for (int s = std::min(maxSurplus, (int)surplusIndices.size()); s >= 1; --s) {
    DEBUG_PRINT("Generating tuples with " << s << " surplus stations");
    for (int d = std::min(maxDeficit, (int)deficitIndices.size()); d >= 1;
         --d) {
      DEBUG_PRINT("Generating tuples with " << d << " deficit stations");
      std::vector<bool> surSelect(surplusIndices.size(), false);
      std::fill(surSelect.begin(), surSelect.begin() + s, true);
      do {
        std::vector<int> surCombo;
        for (size_t i = 0; i < surSelect.size(); ++i)
          if (surSelect[i])
            surCombo.push_back(surplusIndices[i]);

        std::vector<bool> defSelect(deficitIndices.size(), false);
        std::fill(defSelect.begin(), defSelect.begin() + d, true);
        do {
          std::vector<int> defCombo;
          for (size_t j = 0; j < defSelect.size(); ++j)
            if (defSelect[j])
              defCombo.push_back(deficitIndices[j]);

          // Immediate evaluation — note: evaluateTuple now returns only
          // stations with positive transfer!
          auto tuple = evaluateTuple(surCombo, defCombo, stations);

          if (tuple.deltaUDF > 0) {
            std::set<int> usedSurSet(tuple.surplusStationIndices.begin(),
                                     tuple.surplusStationIndices.end());
            std::set<int> usedDefSet(tuple.deficitStationIndices.begin(),
                                     tuple.deficitStationIndices.end());

            // Anti-subset: skip if this (used) tuple is a subset of any
            // previous (used) tuple
            bool isSubset = false;
            for (size_t k = 0; k < acceptedSurSets.size(); ++k) {
              if (std::includes(acceptedSurSets[k].begin(),
                                acceptedSurSets[k].end(), usedSurSet.begin(),
                                usedSurSet.end()) &&
                  std::includes(acceptedDefSets[k].begin(),
                                acceptedDefSets[k].end(), usedDefSet.begin(),
                                usedDefSet.end())) {
                isSubset = true;
                break;
              }
            }
            if (!isSubset) {
              tuples.push_back(tuple);
              acceptedSurSets.push_back(usedSurSet);
              acceptedDefSets.push_back(usedDefSet);
            }
          }
        } while (std::prev_permutation(defSelect.begin(), defSelect.end()));
      } while (std::prev_permutation(surSelect.begin(), surSelect.end()));
    }
  }
}

TransferTuple
TupleClusterEvaluator::evaluateTuple(const std::vector<int> &surplusIndices,
                                     const std::vector<int> &deficitIndices,
                                     const std::vector<Station> &stations) {
  struct LocalStation {
    int idx;
    int currentInventory;
    int optimalInventory;
    double bcrf;
    const std::vector<double> *udf;
  };

  std::map<std::pair<int, int>, int> bikeAllocations;

  std::vector<LocalStation> surplus, deficit;
  for (int idx : surplusIndices)
    surplus.push_back({idx, stations[idx].getCurrentInventory(),
                       stations[idx].getOptimalInventory(),
                       stations[idx].getBcrf(), &stations[idx].getUdfValues()});
  for (int idx : deficitIndices)
    deficit.push_back({idx, stations[idx].getCurrentInventory(),
                       stations[idx].getOptimalInventory(),
                       stations[idx].getBcrf(), &stations[idx].getUdfValues()});

  std::sort(surplus.begin(), surplus.end(),
            [](const LocalStation &a, const LocalStation &b) {
              return a.bcrf > b.bcrf;
            });
  std::sort(deficit.begin(), deficit.end(),
            [](const LocalStation &a, const LocalStation &b) {
              return a.bcrf > b.bcrf;
            });

  double deltaUDF = 0.0;
  std::vector<int> surplusMoved(surplus.size(), 0),
      deficitMoved(deficit.size(), 0);

  for (size_t si = 0; si < surplus.size(); ++si) {
    auto &s = surplus[si];
    int available = s.currentInventory - s.optimalInventory;
    if (available <= 0)
      continue;
    for (size_t di = 0; di < deficit.size(); ++di) {
      auto &d = deficit[di];
      int needed = d.optimalInventory - d.currentInventory;
      if (needed <= 0)
        continue;
      int transfer = std::min(available, needed);
      if (transfer <= 0)
        continue;

      double surDelta = 0.0, defDelta = 0.0;
      for (int k = 0; k < transfer; ++k) {
        surDelta += (*s.udf)[s.currentInventory - k] -
                    (*s.udf)[s.currentInventory - k - 1];
        defDelta += (*d.udf)[d.currentInventory + k] -
                    (*d.udf)[d.currentInventory + k + 1];
      }
      deltaUDF += surDelta + defDelta;
      surplusMoved[si] += transfer;
      deficitMoved[di] += transfer;

      s.currentInventory -= transfer;
      d.currentInventory += transfer;
      // add the transfer to the bikeAllocations
      bikeAllocations[std::make_pair(s.idx, d.idx)] = transfer;
      available -= transfer;
      if (available <= 0)
        break;
    }
  }

  std::vector<int> usedSurplus, usedDeficit;
  for (size_t si = 0; si < surplus.size(); ++si)
    if (surplusMoved[si] > 0)
      usedSurplus.push_back(surplus[si].idx);
  for (size_t di = 0; di < deficit.size(); ++di)
    if (deficitMoved[di] > 0)
      usedDeficit.push_back(deficit[di].idx);

  TransferTuple tuple;
  tuple.surplusStationIndices = usedSurplus;
  tuple.deficitStationIndices = usedDeficit;
  tuple.bikeAllocations = bikeAllocations;
  tuple.deltaUDF = deltaUDF;
  return tuple;
}

// Main greedy selector
std::vector<TransferTuple> TupleClusterEvaluator::greedySelectExclusiveTuples(
    const std::vector<TransferTuple> &tuples) {

  // Sort tuples by deltaUDF descending (break ties by tuple size descending)
  std::vector<TransferTuple> sortedTuples = tuples;
  std::sort(sortedTuples.begin(), sortedTuples.end(),
            [](const TransferTuple &a, const TransferTuple &b) {
              if (a.deltaUDF != b.deltaUDF)
                return a.deltaUDF > b.deltaUDF;
              size_t sizeA = a.surplusStationIndices.size() +
                             a.deficitStationIndices.size();
              size_t sizeB = b.surplusStationIndices.size() +
                             b.deficitStationIndices.size();
              return sizeA > sizeB;
            });
  DEBUG_PRINT("sort OK");
  std::vector<TransferTuple> selected;
  std::set<int> usedStations; // IDs of stations already assigned

  for (const auto &tuple : sortedTuples) {
    bool conflict = false;
    // Check surplus
    for (int sid : tuple.surplusStationIndices) {
      if (usedStations.count(sid)) {
        conflict = true;
        break;
      }
    }
    if (conflict)
      continue;
    // Check deficit
    for (int did : tuple.deficitStationIndices) {
      if (usedStations.count(did)) {
        conflict = true;
        break;
      }
    }
    if (conflict)
      continue;

    // If no conflict, select this tuple
    selected.push_back(tuple);
    for (int sid : tuple.surplusStationIndices)
      usedStations.insert(sid);
    for (int did : tuple.deficitStationIndices)
      usedStations.insert(did);
  }

  return selected;
}
