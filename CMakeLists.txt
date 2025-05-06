cmake_minimum_required(VERSION 3.10)
project(BRP-cluster)
# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SKIP_RPATH TRUE)

# Enable automatic rebuilding
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Set library paths to use system libraries
set(CMAKE_LIBRARY_PATH /usr/lib/x86_64-linux-gnu)
set(CMAKE_PREFIX_PATH /usr)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set explicit library search paths
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--disable-new-dtags")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--disable-new-dtags")

# Find required packages
find_package(GTest REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add main library
add_library(BRP-core
    src/Core/ProblemInstance.cpp
    src/Core/Station.cpp
    src/Core/Param.cpp
    src/Utils/MetricCalculator.cpp
    src/KMedoid/MedoidInitializer.cpp
)

# Add include directories for main library
target_include_directories(BRP-core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

# Link main library with required libraries
target_link_libraries(BRP-core PUBLIC
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# Add test executable
add_executable(ReadStationsTest
    tests/ReadStationsTest.cpp
)

# Link test executable with required libraries
target_link_libraries(ReadStationsTest
    BRP-core
)