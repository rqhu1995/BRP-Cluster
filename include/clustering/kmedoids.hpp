#pragma once

#include "../core/station.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class KMedoid {
public:
  // Constructor
  KMedoid(const std::vector<Station> &stations, int k);

  // Initialization methods
  std::vector<int> initMedoidsBCRF();
  std::vector<int> initMedoidsBalanced();
  std::vector<int> initMedoidsDispersion();

  // Core clustering methods
  std::vector<std::vector<int>>
  assignToClusters(const std::vector<int> &medoids);
  std::vector<int> updateMedoids(const std::vector<std::vector<int>> &clusters);

  // Main clustering method
  std::vector<std::vector<int>> run(double lambda,
                                    double convergenceThreshold = 1e-6,
                                    int maxIterations = 1000);

  // Setters
  void
  setCompositeDistanceMatrix(const std::vector<std::vector<double>> &matrix);
  void setK(int k);

private:
  const std::vector<Station> &stations;
  int k;
  std::vector<std::vector<double>> compositeDistance;
};