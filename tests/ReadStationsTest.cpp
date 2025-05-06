#include "Core/ProblemInstance.hpp"
#include "Utils/MetricCalculator.hpp"
#include <iostream>

using namespace MetricCalculator;

int main() {
  ProblemInstance instance("../data/results.csv");

  std::cout << "Stations loaded: " << instance.getStations().size() << "\n";

  // system analysis, check the distribution of stations in the dataset
  // (currentIventory > optimalInventory -> Surplus Stations; currentInventory <
  // optimalInventory -> Deficit Stations, currentInventory == optimalInventory
  // -> Balanced Stations) print out the number of surplus, deficit, and
  // balanced stations
  int surplus = 0;
  int deficit = 0;
  int balanced = 0;
  for (const auto &station : instance.getStations()) {
    if (station.getCurrentInventory() > station.getOptimalInventory()) {
      surplus++;
    } else if (station.getCurrentInventory() < station.getOptimalInventory()) {
      deficit++;
    } else {
      balanced++;
    }
  }
  std::cout << "Surplus Stations: " << surplus << "\n";
  std::cout << "Deficit Stations: " << deficit << "\n";
  std::cout << "Balanced Stations: " << balanced << "\n";

  Param param(60, 0.7, 0.3, 10, 10, 10);
  MetricCalculator::computeBCRF(instance.getStations(), param);
  MetricCalculator::computeCompositeDistanceMatrix(
      instance.getStations(), instance.getTimeMatrix(), 0.7, 0.3);
  return 0;
}
