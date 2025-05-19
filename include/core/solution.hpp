#pragma once

#include "Data/Station.hpp"
#include <vector>

class Solution {
public:
  Solution() = default;

  void addStation(const Station &station) { stations.push_back(station); }

  const std::vector<Station> &getStations() const { return stations; }

private:
  std::vector<Station> stations;
};