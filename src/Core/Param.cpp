#include "Core/Param.hpp"

Param::Param(double tLoad, double alpha, double beta, int numOfStations,
             int numOfClusters, int numOfVehicles)
    : tLoad(tLoad), alpha(alpha), beta(beta), numOfStations(numOfStations),
      numOfClusters(numOfClusters), numOfVehicles(numOfVehicles) {}
