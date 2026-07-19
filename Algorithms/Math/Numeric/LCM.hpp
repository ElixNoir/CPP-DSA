#pragma once

#include "GCD.hpp"

namespace Math {

  namespace Numeric {

    size_t lcm_binary(size_t a, size_t b) {
      if (a == 0 || b == 0) return 0;
      return (a / gcd_binary(a, b)) * b; 
    }

    size_t lcm_euclidean(size_t a, size_t b) {
      if (a == 0 || b == 0) return 0;
      return (a / gcd_euclidean(a, b)) * b; 
    }

  }

}
