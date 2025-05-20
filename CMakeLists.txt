cmake_minimum_required(VERSION 3.10)
project(BRP-cluster)
# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SKIP_RPATH TRUE)

# Enable debug mode and add debug flags
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra -Wno-sign-compare")

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
    src/core/problem.cpp
    src/core/station.cpp
    src/core/param.cpp
    src/utils/metric.cpp
    src/clustering/kmedoids.cpp
    src/clustering/tuple_evaluator.cpp
)

# Add include directories for main library
target_include_directories(BRP-core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/Utils
    ${CURL_INCLUDE_DIRS}
)

# Link main library with required libraries
target_link_libraries(BRP-core PUBLIC
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# Add test executable
add_executable(tuple_evaluation_test
    tests/tuple_evaluation_test.cpp
)

# Link test executable with required libraries
target_link_libraries(tuple_evaluation_test
    BRP-core
    GTest::GTest
    GTest::Main
)

# Enable testing
# enable_testing()
# add_test(NAME tuple_evaluation_test COMMAND tuple_evaluation_test)