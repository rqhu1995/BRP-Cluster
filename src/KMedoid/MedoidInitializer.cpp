#include "../include/KMedoid/MedoidInitializer.hpp"
#include <algorithm>
#include <unordered_map>
#include <vector>

#include "../include/Core/Station.hpp"

namespace KMedoid {

std::vector<Coordinate> initMedoidsBCRF(const std::vector<Station> &stations,
                                        int k) {
  // setting the medoids to the k stations with the highest BCRF
  // srot the stations by BCRF
  std::vector<Station> sorted_stations = stations;
  std::sort(sorted_stations.begin(), sorted_stations.end(),
            [](const Station &a, const Station &b) {
              return a.getBcrf() > b.getBcrf();
            });
  std::vector<Coordinate> medoids;
  for (int i = 0; i < k; i++) {
    medoids.push_back(sorted_stations[i].getCoordinate());
  }
  return medoids;
}

std::vector<Coordinate>
initMedoidsBalanced(const std::vector<Station> &stations, int k) {
  // select top k/2 surplus stations and top k/2 deficit stations
  // sort the stations by currentInventory - optimalInventory in descending
  // order
  std::vector<Station> sorted_stations = stations;
  std::vector<Coordinate> medoids;
  std::sort(sorted_stations.begin(), sorted_stations.end(),
            [](const Station &a, const Station &b) {
              return a.getCurrentInventory() - a.getOptimalInventory() >
                     b.getCurrentInventory() - b.getOptimalInventory();
            });
  // pick up the top k/2 as the top k/2 surplus stations and the bottom k/2 as
  // the bottom k/2 deficit stations
  for (int i = 0; i < k / 2; i++) {
    medoids.push_back(sorted_stations[i].getCoordinate());
  }
  for (int i = sorted_stations.size() - k / 2; i < sorted_stations.size();
       i++) {
    medoids.push_back(sorted_stations[i].getCoordinate());
  }

  return medoids;
}

std::vector<Coordinate> initMedoidsDispersion(
    const std::vector<Station> &stations, int k,
    const std::vector<std::vector<double>> &compositeDistanceMatrix) {
  std::vector<Station> sorted_stations = stations;
  // Select first medoid as station with highest BCRF
  std::sort(sorted_stations.begin(), sorted_stations.end(),
            [](const Station &a, const Station &b) {
              return a.getBcrf() > b.getBcrf();
            });
  std::vector<Coordinate> medoids;
  medoids.push_back(sorted_stations[0].getCoordinate());
  /** Iteratively select each new medoid as the station that maximizes the
  minimum composite distance to all previously selected medoids */
  for (int i = 1; i < k; i++) {
    double max_min_distance = -1;
    int max_min_distance_index = -1;
    for (int j = 0; j < sorted_stations.size(); j++) {
      double min_distance = 1e9;
      for (int m = 0; m < medoids.size(); m++) {
        min_distance = std::min(min_distance, compositeDistanceMatrix[j][m]);
      }
      if (min_distance > max_min_distance) {
        max_min_distance = min_distance;
        max_min_distance_index = j;
      }
    }
    medoids.push_back(sorted_stations[max_min_distance_index].getCoordinate());
  }
  return medoids;
}
} // namespace KMedoid