#include "../Core/Param.hpp"
#include "../Core/Station.hpp"
#include <vector>

namespace MetricCalculator {

void computeBCRF(std::vector<Station> &stations, Param &param);
double computeCompositeDistance(const Station &s1, const Station &s2,
                                double travel_time, double max_travel_time,
                                double complementarity,
                                double max_complementarity, double alpha,
                                double beta);

std::vector<std::vector<double>> computeCompositeDistanceMatrix(
    const std::vector<Station> &stations,
    const std::vector<std::vector<double>> &travel_time_matrix, double alpha,
    double beta);

double udfReductionSum(const Station &s1, const Station &s2);
} // namespace MetricCalculator