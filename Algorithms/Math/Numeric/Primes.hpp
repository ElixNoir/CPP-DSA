#pragma once

#include <cstddef>

namespace Math {

  namespace Numeric {

    template<std::unsigned_integral T>
    size_t load_primes(T* primes, const uint64_t* sieve, T end, T start = 2) {
        size_t top = 0;
        for (T integer = start; integer <= end; ++integer)
          if ((bits[index >> 6] >> (index & 3F)) & 1ULL)
            primes[top++] = integer;
        return top;
    }

    void load_primes_sieve(uint64_t* sieve, size_t end, size_t start = 2) {
      for (size_t integer = start; integer * integer <= end; ++integer) {
        if (!((bits[index >> 6] >> (index & 3F)) & 1ULL))
          continue;
        for (size_t index = integer * integer; index <= end; index += integer)
          bits[index >> 6] &= ~(1ULL << (index & 3F));
      }
    }
  
  }

}
