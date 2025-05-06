#include "Core/ProblemInstance.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

class ProblemInstanceTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create a temporary test CSV file
    testFile = "test_stations.csv";
    std::ofstream file(testFile);
    file << "sys_id,name,latitude,longitude,capacity,current_inventory,optimal_"
            "inventory,min_udf,udf_values\n";
    file << "station1,Station "
            "1,13.388860,52.517037,10,5,7,0.5,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,"
            "1.3,1.4\n";
    file << "station2,Station "
            "2,13.397634,52.529407,8,4,6,0.4,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1\n";
    file.close();
  }

  void TearDown() override {
    // Clean up the temporary file
    std::filesystem::remove(testFile);
  }

  std::string testFile;
};

TEST_F(ProblemInstanceTest, ConstructorLoadsStations) {
  ProblemInstance instance(testFile);

  // Check if stations were loaded correctly
  EXPECT_EQ(instance.getStations().size(), 3); // 2 stations + depot

  // Check depot properties
  const auto &depot = instance.getStations()[0];
  EXPECT_EQ(depot.getSysId(), "depot");
  EXPECT_EQ(depot.getId(), 0);

  // Check first station properties
  const auto &station1 = instance.getStations()[1];
  EXPECT_EQ(station1.getSysId(), "station1");
  EXPECT_EQ(station1.getCapacity(), 10);
  EXPECT_EQ(station1.getCurrentInventory(), 5);
  EXPECT_EQ(station1.getOptimalInventory(), 7);

  // Check second station properties
  const auto &station2 = instance.getStations()[2];
  EXPECT_EQ(station2.getSysId(), "station2");
  EXPECT_EQ(station2.getCapacity(), 8);
  EXPECT_EQ(station2.getCurrentInventory(), 4);
  EXPECT_EQ(station2.getOptimalInventory(), 6);
}

TEST_F(ProblemInstanceTest, TimeMatrixIsComputed) {
  ProblemInstance instance(testFile);

  // Check if time matrix was computed
  const auto &timeMatrix = instance.getTimeMatrix();
  EXPECT_EQ(timeMatrix.size(), 3); // 3x3 matrix (2 stations + depot)
  EXPECT_EQ(timeMatrix[0].size(), 3);

  // Check if all values are non-negative
  for (const auto &row : timeMatrix) {
    for (const auto &value : row) {
      EXPECT_GE(value, 0.0);
    }
  }
}