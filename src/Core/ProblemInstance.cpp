#include "Core/ProblemInstance.hpp"
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <sstream>

// Forward declaration
void saveMatrixToFile(const std::vector<Station> &stations,
                      const std::vector<std::vector<double>> &timeMatrix);
double euclideanDistance(const Coordinate &coord1, const Coordinate &coord2);
// Callback function for CURL
static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            std::string *userp) {
  userp->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string sendRequest(const std::string &url) {
  CURL *curl = curl_easy_init();
  std::string response;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      throw std::runtime_error("CURL request failed: " +
                               std::string(curl_easy_strerror(res)));
    }

    curl_easy_cleanup(curl);
  }
  return response;
}

void parseResponse(const std::string &response,
                   std::vector<std::vector<double>> &timeMatrix) {
  auto json = nlohmann::json::parse(response);
  auto durations = json["durations"];

  for (size_t i = 0; i < durations.size(); ++i) {
    for (size_t j = 0; j < durations[i].size(); ++j) {
      timeMatrix[i][j] = durations[i][j];
    }
  }
}

ProblemInstance::ProblemInstance(const std::string &filename) {
  // TODO: Implement station loading logic
  std::ifstream file(filename);
  std::string line;

  // Skip header line
  std::getline(file, line);
  int stationId = 1;
  std::cout << "Loading station information..." << std::endl;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string field;

    // Parse CSV fields
    std::string sysId;
    std::string name;
    double latitude;
    double longitude;
    int capacity;
    int currentInventory;
    int optimalInventory;
    double minUdf;
    std::vector<double> udfValues;

    // Read fields
    std::getline(ss, sysId, ',');
    std::getline(ss, name, ',');
    ss >> latitude;
    ss.ignore(); // Skip comma
    ss >> longitude;
    ss.ignore();
    ss >> capacity;
    ss.ignore();
    ss >> currentInventory;
    ss.ignore();
    ss >> optimalInventory;
    ss.ignore();
    ss >> minUdf;
    ss.ignore();

    // Read UDF values, the UDF value list should be indexed from 0 to the
    // capacity, read the value one by one until the capacity is reached
    for (int i = 0; i < capacity; i++) {
      double udfValue;
      ss >> udfValue;
      udfValues.push_back(udfValue);
      ss.ignore();
    }

    // Create station and add to vector
    Station station(sysId, stationId, Coordinate(latitude, longitude), capacity,
                    currentInventory, optimalInventory, udfValues);
    stations.push_back(station);
    stationId++;
  }

  std::cout << "Computing Station Centroid..." << std::endl;

  //   Compute the centroid of the network, then add this as the 0th station
  //   to the vector with sys_id being "depot", capacity/inventory/optimal all
  //   set to infinity
  double totalLatitude = 0.0;
  double totalLongitude = 0.0;
  for (const auto &station : stations) {
    totalLatitude += station.getCoordinate().latitude;
    totalLongitude += station.getCoordinate().longitude;
  }
  double centroidLatitude = totalLatitude / stations.size();
  double centroidLongitude = totalLongitude / stations.size();
  Station depot("depot", 0, Coordinate(centroidLatitude, centroidLongitude),
                std::numeric_limits<int>::max(),
                std::numeric_limits<int>::max(),
                std::numeric_limits<int>::max(), std::vector<double>());
  stations.insert(stations.begin(), depot);

  std::cout << "Computing Duration Matrix..." << std::endl;

  /**
  // Process in chunks of 25 stations (OSRM's safe limit)
  const size_t CHUNK_SIZE = 25;
  const size_t NUM_CHUNKS = (stations.size() + CHUNK_SIZE - 1) / CHUNK_SIZE;

  std::cout << "Processing " << stations.size() << " stations in " << NUM_CHUNKS
            << " chunks of " << CHUNK_SIZE << " stations each" << std::endl;

  // Process all chunk pairs
  for (size_t i = 0; i < stations.size(); i += CHUNK_SIZE) {
    size_t i_end = std::min(i + CHUNK_SIZE, stations.size());

    for (size_t j = 0; j < stations.size(); j += CHUNK_SIZE) {
      size_t j_end = std::min(j + CHUNK_SIZE, stations.size());

      // Check if this chunk pair is already fully computed
      bool chunk_pair_computed = true;
      for (size_t k = i; k < i_end && chunk_pair_computed; ++k) {
        for (size_t l = j; l < j_end && chunk_pair_computed; ++l) {
          if (timeMatrix[k][l] == 0.0) {
            chunk_pair_computed = false;
          }
        }
      }

      if (chunk_pair_computed) {
        std::cout << "Skipping chunk pair " << i / CHUNK_SIZE + 1 << "→"
                  << j / CHUNK_SIZE + 1 << " (already computed)" << std::endl;
        continue;
      }

      // Build URL for this chunk pair
      std::string url = "http://router.project-osrm.org/table/v1/driving/";

      // Add source stations (i chunk)
      for (size_t k = i; k < i_end; ++k) {
        url += std::to_string(stations[k].getCoordinate().longitude) + "," +
               std::to_string(stations[k].getCoordinate().latitude) + ";";
      }

      // Add destination stations (j chunk)
      for (size_t k = j; k < j_end - 1; ++k) {
        url += std::to_string(stations[k].getCoordinate().longitude) + "," +
               std::to_string(stations[k].getCoordinate().latitude) + ";";
      }
      url += std::to_string(stations[j_end - 1].getCoordinate().longitude) +
             "," + std::to_string(stations[j_end - 1].getCoordinate().latitude);

      std::cout << "Processing chunk pair " << i / CHUNK_SIZE + 1 << "→"
                << j / CHUNK_SIZE + 1 << " of " << NUM_CHUNKS << "×"
                << NUM_CHUNKS << std::endl;

      // Get and parse response
      std::string response = sendRequest(url);
      auto json = nlohmann::json::parse(response);

      if (json["code"] != "Ok") {
        throw std::runtime_error("OSRM API error: " +
                                 json["message"].get<std::string>());
      }

      auto durations = json["durations"];

      // Copy results to the appropriate positions in timeMatrix
      for (size_t k = 0; k < i_end - i; ++k) {
        for (size_t l = 0; l < j_end - j; ++l) {
          timeMatrix[i + k][j + l] = durations[k][l];
        }
      }

      // Save progress after each chunk pair
      saveMatrixToFile(stations, timeMatrix);
    }
  }

  std::cout << "Time matrix computation completed" << std::endl;
**/
  // use euclidean distance for now, assuming the truck speed is 25.2km/h
  // Initialize time matrix
  timeMatrix.resize(stations.size(), std::vector<double>(stations.size(), 0.0));

  // Try to load existing matrix if available
  std::ifstream matrixFile("time_matrix_ed.csv");
  if (matrixFile.is_open()) {
    std::cout << "Loading existing time matrix..." << std::endl;
    std::string line;
    // Skip header
    std::getline(matrixFile, line);

    for (size_t i = 0; i < stations.size(); ++i) {
      std::getline(matrixFile, line);
      std::stringstream ss(line);
      std::string stationId;
      std::getline(ss, stationId, ',');

      for (size_t j = 0; j < stations.size(); ++j) {
        std::string value;
        std::getline(ss, value, ',');
        if (!value.empty()) {
          timeMatrix[i][j] = std::stod(value);
        }
      }
    }
    std::cout << "Existing matrix loaded" << std::endl;
  } else {
    for (size_t i = 0; i < stations.size(); ++i) {
      for (size_t j = 0; j < stations.size(); ++j) {
        std::cout << "Computing distance between "
                  << stations[i].getCoordinate().latitude << " and "
                  << stations[j].getCoordinate().longitude << std::endl;
        timeMatrix[i][j] = euclideanDistance(stations[i].getCoordinate(),
                                             stations[j].getCoordinate()) /
                           25.2;
      }
    }
    saveMatrixToFile(stations, timeMatrix);
  }
}

double euclideanDistance(const Coordinate &coord1, const Coordinate &coord2) {
  return std::sqrt(std::pow(coord1.longitude - coord2.longitude, 2) +
                   std::pow(coord1.latitude - coord2.latitude, 2));
}

// Helper function to save matrix to file
void saveMatrixToFile(const std::vector<Station> &stations,
                      const std::vector<std::vector<double>> &timeMatrix) {
  std::ofstream outFile("time_matrix_ed.csv");
  if (!outFile.is_open()) {
    throw std::runtime_error("Failed to open time_matrix_ed.csv for writing");
  }

  // Write header with station IDs
  outFile << "From/To";
  for (const auto &station : stations) {
    outFile << "," << station.getSysId();
  }
  outFile << "\n";

  // Write matrix data
  for (size_t i = 0; i < stations.size(); ++i) {
    outFile << stations[i].getSysId();
    for (size_t j = 0; j < stations.size(); ++j) {
      outFile << "," << timeMatrix[i][j];
    }
    outFile << "\n";
  }

  std::cout << "Progress saved" << std::endl;
}

std::vector<Station> &ProblemInstance::getStations() { return stations; }

const std::vector<std::vector<double>> &ProblemInstance::getTimeMatrix() const {
  return timeMatrix;
}

const std::vector<TransferTuple> &ProblemInstance::getTransfers() const {
  return transfers;
}
