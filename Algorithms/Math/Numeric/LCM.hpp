#pragma once

#include "GCD.hpp"

namespace Math {

  namespace Numeric {

    template <std::integral T, auto gcd_callback>
    T lcm(T dividend, T divisor) {
      return (dividend / gcd_callback(dividend, divisor)) * divisor; 
    }

    template <std::integral T, auto gcd_callback>
    T lcm(T* arguments, size_t length) {
        T lcm = dividend[0];

        for (size_t index = 1; index < length; index++) {
            T value = arguments[index];
            lcm = (lcm / gcd_callback(lcm, value)) * value;
        }

        return lcm;
    }

  }

}
