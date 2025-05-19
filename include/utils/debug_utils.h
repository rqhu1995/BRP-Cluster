#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#define DEBUG_MODE 1

#if DEBUG_MODE
#include <iostream>
#define DEBUG_PRINT(x) std::cout << "[DEBUG] " << x << std::endl
#define DEBUG_HERE()                                                           \
  std::cout << "[DEBUG] Reached " << __FILE__ << ":" << __LINE__ << std::endl
#define DEBUG_PRINT_VAR(x)                                                     \
  std::cout << "[DEBUG] " << #x << " = " << x << std::endl
#else
#define DEBUG_PRINT(x)
#define DEBUG_HERE()
#define DEBUG_PRINT_VAR(x)
#endif

#endif // DEBUG_UTILS_H