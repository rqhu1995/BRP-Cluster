#include "core/station.hpp"

Station::Station(std::string sysId, int id, const Coordinate &coordinate,
                 int capacity, int currentInventory, int optimalInventory,
                 const std::vector<double> &udfValues)
    : sysId(sysId), id(id), capacity(capacity),
      currentInventory(currentInventory), optimalInventory(optimalInventory),
      udfValues(udfValues), coordinate(coordinate) {}

void Station::setBcrf(double bcrf) { this->bcrf = bcrf; }

double Station::getBcrf() const { return bcrf; }

int Station::getId() const { return id; }

std::string Station::getSysId() const { return sysId; }

const Coordinate &Station::getCoordinate() const { return coordinate; }

int Station::getCapacity() const { return capacity; }

int Station::getCurrentInventory() const { return currentInventory; }

int Station::getOptimalInventory() const { return optimalInventory; }

const std::vector<double> &Station::getUdfValues() const { return udfValues; }

StationStatus Station::getStatus() const {
  if (currentInventory > optimalInventory) {
    return StationStatus::SURPLUS;
  } else if (currentInventory < optimalInventory) {
    return StationStatus::DEFICIT;
  }
  return StationStatus::BALANCED;
}
