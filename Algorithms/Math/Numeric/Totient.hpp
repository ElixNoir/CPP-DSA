#pragma once

#include <cstdint>
#include <vector>

namespace Math {

  namespace Numeric {

    static std::vector<size_t> Phi = {0, 1};
    static std::vector<bool> Composite;
    static std::vector<size_t> Primes;

    static void build_euler_totient(size_t n) {
      if (n < Phi.size()) return;

      size_t old = Phi.size();

      Phi.resize(n + 1);
      Composite.resize(n + 1);

      for (size_t i = old; i <= n; i++) {
        if (!Composite[i]) {
          Primes.push_back(i);
          Phi[i] = i - 1;
        }

        for (size_t prime : Primes) {
          size_t x = prime * i;
          if (x > n) break;

          Composite[x] = true;

          if (i % prime == 0) {
            Phi[x] = Phi[i] * prime;
            break;
          }
          else Phi[x] = Phi[i] * (prime - 1);
        }
      }
    }
  
  }

}
