#pragma once

#include <string>
#include <vector>

struct Coordinate {
  double latitude;
  double longitude;

  Coordinate(double lat, double lon) : latitude(lat), longitude(lon) {}

  bool operator==(const Coordinate &other) const {
    return latitude == other.latitude && longitude == other.longitude;
  }
};

enum class StationStatus { SURPLUS, DEFICIT, BALANCED };

class Station {
public:
  Station() = default;

  Station(std::string sysId, int id, const Coordinate &coordinate, int capacity,
          int currentInventory, int optimalInventory,
          const std::vector<double> &udfValues);

  std::string getSysId() const;
  int getId() const;
  const Coordinate &getCoordinate() const;
  void setCoordinate(const Coordinate &coord);
  int getCapacity() const;
  int getCurrentInventory() const;
  int getOptimalInventory() const;
  const std::vector<double> &getUdfValues() const;
  double getBcrf() const;
  void setUdfValues(const std::vector<double> &values);
  void setCapacity(int cap);
  void setCurrentInventory(int inv);
  void setBcrf(double bcrf);
  StationStatus getStatus() const;

private:
  std::string sysId;
  int id;
  int capacity;
  int currentInventory;
  int optimalInventory;
  std::vector<double> udfValues;
  Coordinate coordinate{0.0, 0.0}; // Default to (0,0)
  double bcrf;
};