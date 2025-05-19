#include "clustering/kmedoids.hpp"
#include "utils/debug_utils.h"
#include <algorithm>
#include <cmath>
#include <limits>

KMedoid::KMedoid(const std::vector<Station> &stations, int k)
    : stations(stations), k(k) {
  DEBUG_PRINT("KMedoid constructor called with " << stations.size()
                                                 << " stations and k=" << k);
}

void KMedoid::setCompositeDistanceMatrix(
    const std::vector<std::vector<double>> &matrix) {
  DEBUG_PRINT("Setting composite distance matrix of size "
              << matrix.size() << "x"
              << (matrix.empty() ? 0 : matrix[0].size()));
  compositeDistance = matrix;
}

void KMedoid::setK(int k) { this->k = k; }

std::vector<int> KMedoid::initMedoidsBCRF() {
  DEBUG_HERE();
  std::vector<Station> sortedStations = stations;
  std::sort(sortedStations.begin(), sortedStations.end(),
            [](const Station &a, const Station &b) {
              return a.getBcrf() > b.getBcrf();
            });

  std::vector<int> medoids;
  for (int i = 0; i < k && i < static_cast<int>(sortedStations.size()); ++i) {
    for (size_t j = 0; j < stations.size(); ++j) {
      if (stations[j].getCoordinate() == sortedStations[i].getCoordinate()) {
        medoids.push_back(j);
        break;
      }
    }
  }
  return medoids;
}

std::vector<int> KMedoid::initMedoidsBalanced() {
  DEBUG_HERE();
  std::vector<Station> sortedStations = stations;
  std::sort(sortedStations.begin(), sortedStations.end(),
            [](const Station &a, const Station &b) {
              return std::abs(a.getBcrf()) < std::abs(b.getBcrf());
            });

  std::vector<int> medoids;
  for (int i = 0; i < k && i < static_cast<int>(sortedStations.size()); ++i) {
    for (size_t j = 0; j < stations.size(); ++j) {
      if (stations[j].getCoordinate() == sortedStations[i].getCoordinate()) {
        medoids.push_back(j);
        break;
      }
    }
  }
  return medoids;
}

std::vector<int> KMedoid::initMedoidsDispersion() {
  DEBUG_HERE();
  std::vector<Station> sortedStations = stations;
  DEBUG_PRINT("Sorting " << sortedStations.size() << " stations by BCRF");

  std::sort(sortedStations.begin(), sortedStations.end(),
            [](const Station &a, const Station &b) {
              return a.getBcrf() > b.getBcrf();
            });

  std::vector<int> medoids;

  // Verify matrix dimensions
  if (compositeDistance.empty() || compositeDistance[0].empty()) {
    DEBUG_PRINT("ERROR: Empty composite distance matrix");
    return medoids;
  }

  // Add the station with highest BCRF as first medoid
  for (size_t i = 0; i < stations.size(); ++i) {
    if (stations[i].getCoordinate() == sortedStations[0].getCoordinate()) {
      medoids.push_back(i);
      DEBUG_PRINT("First medoid added at index " << i);
      break;
    }
  }

  // Find remaining medoids
  for (int i = 1; i < k; i++) {
    DEBUG_PRINT("Finding medoid " << i + 1 << " of " << k);
    double maxMinDistance = -1;
    int maxMinDistanceIndex = -1;

    for (size_t j = 0; j < stations.size(); j++) {
      // Skip if this station is already a medoid
      if (std::find(medoids.begin(), medoids.end(), j) != medoids.end()) {
        continue;
      }

      double minDistance = std::numeric_limits<double>::max();
      bool validDistance = true;

      for (size_t medoidIdx : medoids) {
        if (j >= compositeDistance.size() ||
            medoidIdx >= compositeDistance[j].size()) {
          DEBUG_PRINT("ERROR: Invalid matrix access at j="
                      << j << ", medoidIdx=" << medoidIdx);
          validDistance = false;
          break;
        }
        minDistance = std::min(minDistance, compositeDistance[j][medoidIdx]);
      }

      if (!validDistance) {
        continue;
      }

      if (minDistance > maxMinDistance) {
        maxMinDistance = minDistance;
        maxMinDistanceIndex = j;
      }
    }

    if (maxMinDistanceIndex == -1) {
      DEBUG_PRINT("WARNING: Could not find valid medoid for cluster " << i);
      continue;
    }

    medoids.push_back(maxMinDistanceIndex);
    DEBUG_PRINT("Added medoid at index " << maxMinDistanceIndex);
  }

  if (medoids.size() != k) {
    DEBUG_PRINT("WARNING: Could only initialize "
                << medoids.size() << " medoids out of " << k << " requested");
  }

  return medoids;
}

std::vector<std::vector<int>>
KMedoid::assignToClusters(const std::vector<int> &medoids) {
  DEBUG_HERE();
  DEBUG_PRINT("Assigning " << stations.size() << " stations to "
                           << medoids.size() << " clusters");

  // Initialize clusters with empty vectors
  std::vector<std::vector<int>> clusters(medoids.size());

  // Verify matrix dimensions
  if (compositeDistance.empty() || compositeDistance[0].empty()) {
    DEBUG_PRINT("ERROR: Empty composite distance matrix");
    return clusters;
  }

  for (size_t i = 1; i < stations.size(); ++i) {
    double minDistance = std::numeric_limits<double>::max();
    int closestMedoidIdx = -1;

    for (size_t j = 0; j < medoids.size(); ++j) {
      if (i >= compositeDistance.size() ||
          medoids[j] >= compositeDistance[i].size()) {
        DEBUG_PRINT("ERROR: Invalid matrix access at i=" << i << ", medoid="
                                                         << medoids[j]);
        continue;
      }
      double distance = compositeDistance[i][medoids[j]];
      if (distance < minDistance) {
        minDistance = distance;
        closestMedoidIdx = j; // Store the cluster index, not the medoid index
      }
    }

    if (closestMedoidIdx != -1) {
      clusters[closestMedoidIdx].push_back(i);
    } else {
      DEBUG_PRINT("WARNING: No valid medoid found for station " << i);
    }
  }
  return clusters;
}

std::vector<int>
KMedoid::updateMedoids(const std::vector<std::vector<int>> &clusters) {
  DEBUG_HERE();
  DEBUG_PRINT("Updating medoids for " << clusters.size());

  std::vector<int> newMedoids(clusters.size(), -1);
  for (size_t j = 0; j < clusters.size(); ++j) {
    DEBUG_PRINT("Processing cluster " << j << " with " << clusters[j].size()
                                      << " stations");
    double minCentrality = std::numeric_limits<double>::max();
    int bestStation = -1;

    for (int stationIdx : clusters[j]) {
      if (stationIdx >= stations.size()) {
        DEBUG_PRINT("ERROR: Invalid station index " << stationIdx);
        continue;
      }

      double sumDistances = 0.0;
      for (int otherStationIdx : clusters[j]) {
        if (stationIdx != otherStationIdx) {
          if (stationIdx >= compositeDistance.size() ||
              otherStationIdx >= compositeDistance[stationIdx].size()) {
            DEBUG_PRINT("ERROR: Invalid matrix access at stationIdx="
                        << stationIdx
                        << ", otherStationIdx=" << otherStationIdx);
            continue;
          }
          sumDistances += compositeDistance[stationIdx][otherStationIdx];
        }
      }

      if (sumDistances < minCentrality) {
        minCentrality = sumDistances;
        bestStation = stationIdx;
      }
    }

    if (bestStation == -1) {
      DEBUG_PRINT("ERROR: No valid medoid found for cluster " << j);
      continue;
    }
    newMedoids[j] = bestStation;
  }
  return newMedoids;
}

std::vector<std::vector<int>>
KMedoid::run(double lambda, double convergenceThreshold, int maxIterations) {
  DEBUG_HERE();
  DEBUG_PRINT("Starting KMedoid clustering with lambda="
              << lambda << ", threshold=" << convergenceThreshold
              << ", maxIter=" << maxIterations);

  std::vector<int> currentMedoids = initMedoidsDispersion();
  DEBUG_PRINT("Initial medoids initialized: " << currentMedoids.size());

  std::vector<std::vector<int>> clusters;
  int iteration = 0;
  bool converged = false;

  while (!converged && iteration < maxIterations) {
    DEBUG_PRINT("Iteration " << iteration + 1 << " of " << maxIterations);

    clusters = assignToClusters(currentMedoids);
    DEBUG_PRINT("Clusters assigned");

    std::vector<int> newMedoids = updateMedoids(clusters);
    DEBUG_PRINT("Medoids updated");

    double maxChange = 0.0;
    for (size_t i = 0; i < currentMedoids.size(); ++i) {
      if (i >= newMedoids.size() ||
          currentMedoids[i] >= compositeDistance.size() ||
          newMedoids[i] >= compositeDistance[currentMedoids[i]].size()) {
        DEBUG_PRINT("ERROR: Invalid matrix access in convergence check");
        continue;
      }
      maxChange = std::max(maxChange,
                           compositeDistance[currentMedoids[i]][newMedoids[i]]);
    }

    DEBUG_PRINT("Max change: "
                << maxChange << ", Converged: "
                << (maxChange < convergenceThreshold ? "yes" : "no"));
    converged = maxChange < convergenceThreshold;
    currentMedoids = newMedoids;
    iteration++;
  }

  return clusters;
}