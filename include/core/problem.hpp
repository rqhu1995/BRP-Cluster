#pragma once

#include "core/station.hpp"
#include "core/transfer_tuple.hpp"
#include <vector>

class ProblemInstance {
public:
  ProblemInstance() = default;

  explicit ProblemInstance(const std::string &filename);

  std::vector<Station> &getStations();
  const std::vector<std::vector<double>> &getTimeMatrix() const;
  const std::vector<TransferTuple> &getTransfers() const;

private:
  std::vector<Station> stations;
  std::vector<std::vector<double>> timeMatrix;
  std::vector<TransferTuple> transfers;
};