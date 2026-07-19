#pragma once

#include <cstddef>
#include <cstdint>

namespace Math {

  namespace Numeric {

    template<std::unsigned_integral T>
    size_t load_primes(T* primes, const uint64_t* sieve, T end, T start = 3) {
      size_t top = 0;
      for (T integer = start | 1; integer <= end; integer += 2)
        if ((sieve[integer >> 6] >> (integer & 3F)) & 1ULL)
          primes[top++] = integer;
      return top;
    }

    void load_primes_sieve(uint64_t* sieve, size_t end, size_t start = 3) {
      sieve[0] |= 4;
      for (size_t integer = start | 1; integer * integer <= end; integer += 2) {
        if (!((sieve[integer >> 6] >> (integer & 3F)) & 1ULL)) continue;
        for (size_t index = integer * integer; index <= end; index += integer << 1)
          sieve[index >> 6] &= ~(1ULL << (index & 3F));
      }
    }
  
  }

}
