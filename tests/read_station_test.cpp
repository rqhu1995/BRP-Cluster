#include "clustering/kmedoids.hpp"
#include "core/problem.hpp"
#include "utils/metric.hpp"
#include <cmath> // for std::abs
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace MetricCalculator;

int main() {
  // Load station data
  ProblemInstance instance("../data/results.csv");
  auto stations = instance.getStations();
  std::cout << "Stations loaded: " << stations.size() << "\n";

  return 0;
}
