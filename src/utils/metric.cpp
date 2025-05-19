#include "utils/metric.hpp"
#include <cstdlib>
#include <iostream>

namespace MetricCalculator {

void computeBCRF(std::vector<Station> &stations, Param &param) {
  for (auto &station : stations) {
    if (station.getId() == 0)
      continue;
    int currentInventory = station.getCurrentInventory();
    int optimalInventory = station.getOptimalInventory();
    double currentUDF = station.getUdfValues()[currentInventory];
    double optimalUDF = station.getUdfValues()[optimalInventory];
    double cost = param.tLoad * std::abs(currentInventory - optimalInventory);
    if (cost > 0) {
      double bcrf = (currentUDF - optimalUDF) / cost;
      station.setBcrf(bcrf);
    } else {
      station.setBcrf(0.0);
    }
  }
}

/**
 * Compute the composite distance between two stations
 * norm_travel = travel_time(i,j) / max_travel_time
 * norm_udf = udf_reduction(i,j) / max_udf_reduction
 * composite = alpha * norm_travel - beta * norm_udf
 **/
double computeCompositeDistance(double travel_time, double max_travel_time,
                                double complementarity,
                                double max_complementarity, double alpha,
                                double beta) {
  double norm_travel = travel_time / max_travel_time;
  double norm_udf = complementarity / max_complementarity;
  return alpha * norm_travel - beta * norm_udf;
}

std::vector<std::vector<double>> computeCompositeDistanceMatrix(
    const std::vector<Station> &stations,
    const std::vector<std::vector<double>> &travel_time_matrix, double alpha,
    double beta) {
  // form a |Station| * |Station| matrix
  std::vector<std::vector<double>> composite_distance_matrix(
      stations.size(), std::vector<double>(stations.size(), -1.0));

  std::vector<std::vector<double>> complementarity_matrix(
      stations.size(), std::vector<double>(stations.size(), -1.0));

  // Compute complementarity matrix for actual stations (skip depot)
  for (size_t i = 1; i < stations.size(); ++i) {
    for (size_t j = 1; j < stations.size(); ++j) {
      complementarity_matrix[i][j] = udfReductionSum(stations[i], stations[j]);
    }
  }
  std::cout << "Complementarity matrix computed" << std::endl;

  // Compute composite distances for actual stations (skip depot)
  for (size_t i = 1; i < stations.size(); ++i) {
    double max_travel_time = 0.0;
    double max_complementarity = 0.0;
    for (size_t j = 1; j < stations.size(); ++j) {
      if (travel_time_matrix[i][j] > max_travel_time) {
        max_travel_time = travel_time_matrix[i][j];
      }
      if (complementarity_matrix[i][j] > max_complementarity) {
        max_complementarity = complementarity_matrix[i][j];
      }
    }
    if (max_travel_time == 0.0) {
      max_travel_time = 1.0;
    }
    if (max_complementarity == 0.0) {
      max_complementarity = 1.0;
    }
    for (size_t j = 1; j < stations.size(); ++j) {
      composite_distance_matrix[i][j] = computeCompositeDistance(
          travel_time_matrix[i][j], max_travel_time,
          complementarity_matrix[i][j], max_complementarity, alpha, beta);
    }
  }
  return composite_distance_matrix;
}

double udfReductionSum(const Station &s1, const Station &s2) {
  int currentInvS1 = s1.getCurrentInventory();
  int currentInvS2 = s2.getCurrentInventory();
  int optimalInvS1 = s1.getOptimalInventory();
  int optimalInvS2 = s2.getOptimalInventory();
  bool isSurplusS1 = currentInvS1 > optimalInvS1;
  bool isSurplusS2 = currentInvS2 > optimalInvS2;
  if (currentInvS1 == optimalInvS1 || currentInvS2 == optimalInvS2 ||
      isSurplusS1 == isSurplusS2) {
    return 0.0;
  } else {
    int bikeTransfer = 0;
    if (isSurplusS1) {
      bikeTransfer =
          std::min(currentInvS1 - optimalInvS1, optimalInvS2 - currentInvS2);
      return (s1.getUdfValues()[currentInvS1] -
              s1.getUdfValues()[currentInvS1 - bikeTransfer]) +
             (s2.getUdfValues()[currentInvS2] -
              s2.getUdfValues()[currentInvS2 + bikeTransfer]);
    } else {
      bikeTransfer =
          std::min(optimalInvS1 - currentInvS1, currentInvS2 - optimalInvS2);
      return (s1.getUdfValues()[currentInvS1] -
              s1.getUdfValues()[currentInvS1 + bikeTransfer]) +
             (s2.getUdfValues()[currentInvS2] -
              s2.getUdfValues()[currentInvS2 - bikeTransfer]);
    }
  }
}

void printCompositeDistanceMatrix(
    int num_stations, const std::vector<std::vector<double>> &matrix) {
  for (int i = 0; i < num_stations; ++i) {
    for (int j = 0; j < num_stations; ++j) {
      std::cout << matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
} // namespace MetricCalculator
