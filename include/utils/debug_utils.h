#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#define DEBUG_MODE 1

#if DEBUG_MODE
#include <iostream>
#include <vector>
#define DEBUG_PRINT(x) std::cout << "[DEBUG] " << x << std::endl
#define DEBUG_HERE()                                                           \
  std::cout << "[DEBUG] Reached " << __FILE__ << ":" << __LINE__ << std::endl
#define DEBUG_PRINT_VAR(x)                                                     \
  std::cout << "[DEBUG] " << #x << " = " << x << std::endl
#define DEBUG_PRINT_TUPLE(x)                                                   \
  do {                                                                         \
    std::cout << "[DEBUG] surplus stations [";                                 \
    for (size_t i = 0; i < x.surplusStationIndices.size(); ++i) {              \
      std::cout << x.surplusStationIndices[i];                                 \
      if (i < x.surplusStationIndices.size() - 1)                              \
        std::cout << ", ";                                                     \
    }                                                                          \
    std::cout << "], deficit stations [";                                      \
    for (size_t i = 0; i < x.deficitStationIndices.size(); ++i) {              \
      std::cout << x.deficitStationIndices[i];                                 \
      if (i < x.deficitStationIndices.size() - 1)                              \
        std::cout << ", ";                                                     \
    }                                                                          \
    std::cout << "]" << std::endl;                                             \
  } while (0)

#else
#define DEBUG_PRINT(x)
#define DEBUG_HERE()
#define DEBUG_PRINT_VAR(x)
#define DEBUG_PRINT_TUPLE(x)
#endif

#endif // DEBUG_UTILS_H