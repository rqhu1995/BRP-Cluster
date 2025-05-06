class Param {
public:
  double tLoad;
  double alpha;
  double beta;
  int numOfStations;
  int numOfClusters;
  int numOfVehicles;

  Param(double tLoad, double alpha, double beta, int numOfStations,
        int numOfClusters, int numOfVehicles);
};