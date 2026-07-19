#pragma once

#include <cstddef>

namespace Math {

  namespace Numeric {

    void load_primes(T* primes, size_t end, size_t start = 0) {
      size_t top = 0;
      for (size_t integer = start; integer <= end; integer++)
        if (primesSieve[integer])
          primes[top++] = integer;
    }

    void load_primes_sieve(T* primesSieve, size_t end, size_t start = 0) {
      for (size_t integer = start; integer * integer <= end; integer++)
        if (primesSieve[integer] != 0)
          for (size_t index = integer * integer; index <= end; index += integer)
            primesSieve[index] = false;
    }
  
  }

}
