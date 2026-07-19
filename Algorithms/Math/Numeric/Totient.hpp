#pragma once

#include "Algorithms/Math/Numeric/Primes.hpp"

namespace Math {

  namespace Numeric {

    template <std::unsigned_integral T, std::unsigned_integral U>
    void load_euler_totient(T* phi, const U* primes, const uint64_t* sieve, U end, U start = 3) {
      for (U integer = start | 1; integer <= end; integer += 2)
        if (!((sieve[integer >> 6] >> (integer & 0x3F)) & 1ULL)) continue;
        for (U index = integer; index <= end; index += integer << 1) 
          phi[index] -= phi[index] / integer;
      }
    }
  
  }

}
