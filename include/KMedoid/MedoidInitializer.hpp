#pragma once

#include <unordered_map>
#include <vector>

#include "../Core/Station.hpp"

namespace KMedoid {
std::vector<Coordinate> initMedoidsBCRF(const std::vector<Station> &stations,
                                        int k);
std::vector<Coordinate>
initMedoidsBalanced(const std::vector<Station> &stations, int k);
std::vector<Coordinate> initMedoidsDispersion(
    const std::vector<Station> &stations, int k,
    const std::vector<std::vector<double>> &compositeDistanceMatrix);
} // namespace KMedoid