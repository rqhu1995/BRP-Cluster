#include "clustering/kmedoids.hpp"
#include "core/problem.hpp"
#include "utils/metric.hpp"
#include <fstream>
#include <iostream>
#include <vector>

int main() {
  ProblemInstance instance("../data/results.csv");
  auto stations = instance.getStations();
  // Calculate BCRF and composite distance matrix
  Param param(60, 2, 0.5, 10, 10, 10);
  MetricCalculator::computeBCRF(stations, param);
  auto compositeDistance = MetricCalculator::computeCompositeDistanceMatrix(
      stations, instance.getTimeMatrix(), 2, 0.5);

  // Perform KMedoid clustering
  const int numClusters = 3; // You can adjust this number
  KMedoid kmedoid(stations, numClusters);
  kmedoid.setCompositeDistanceMatrix(compositeDistance);
  // Run clustering with lambda = 0.5
  auto clusters = kmedoid.run(0.5);

  // Print clustering results
  std::cout << "Clustering Results:\n";
  for (size_t i = 0; i < clusters.size(); ++i) {
    int clusterSurplus = 0;
    int clusterDeficit = 0;
    int clusterBalanced = 0;
    int totalInventoryDiff = 0;
    double avgBcrf = 0.0;

    std::cout << "Cluster " << i + 1 << " (" << clusters[i].size()
              << " stations):\n";
    for (int stationIdx : clusters[i]) {
      const auto &station = stations[stationIdx];
      int inventoryDiff =
          station.getCurrentInventory() - station.getOptimalInventory();
      totalInventoryDiff += std::abs(inventoryDiff);
      avgBcrf += station.getBcrf();

      if (inventoryDiff > 0)
        clusterSurplus++;
      else if (inventoryDiff < 0)
        clusterDeficit++;
      else
        clusterBalanced++;
    }
    avgBcrf /= clusters[i].size();
    std::cout << "  Cluster Summary:\n"
              << "    Surplus stations: " << clusterSurplus << "\n"
              << "    Deficit stations: " << clusterDeficit << "\n"
              << "    Balanced stations: " << clusterBalanced << "\n"
              << "    Total inventory difference: " << totalInventoryDiff
              << "\n    Average BCRF: " << avgBcrf << "\n\n";
  }

  // Save results to CSV for visualization
  std::ofstream outFile("real_stations_clustering.csv");
  outFile << "Station,Latitude,Longitude,Cluster,CurrentInventory,"
             "OptimalInventory,InventoryDiff,BCRF\n";
  for (size_t i = 0; i < clusters.size(); ++i) {
    for (int stationIdx : clusters[i]) {
      const auto &station = stations[stationIdx];
      double inventoryDiff =
          station.getCurrentInventory() - station.getOptimalInventory();
      outFile << stationIdx << "," << station.getCoordinate().latitude << ","
              << station.getCoordinate().longitude << "," << i + 1 << ","
              << station.getCurrentInventory() << ","
              << station.getOptimalInventory() << "," << inventoryDiff << ","
              << station.getBcrf() << "\n";
    }
  }
  outFile.close();

  std::cout << "Results have been saved to real_stations_clustering.csv\n";
}
