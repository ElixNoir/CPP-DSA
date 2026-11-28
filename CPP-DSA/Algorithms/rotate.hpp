#pragma once

#include "swap.hpp"

namespace Algorithms {

  using std::size_t;

  template <typename T>
    requires std::is_swappable_v<T>
  constexpr void rotate(T* first, size_t left, size_t right)
      noexcept (std::is_nothrow_swappable_v<T>)
  {
    while (left && right) {
      if (left <= right) {
        const size_t other = first + left;
        swap_many(first, other, other);
        first = other;
        right -= left;
      }
      else {
        swap_many(first + (left - right), first + left, right);
        left -= right;
      }
    }
  }

}
