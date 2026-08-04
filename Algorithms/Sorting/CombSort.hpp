#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#include <cstdint>

#pragma endregion

template <Comparable T, std::unsigned_integral Index = uint32_t>
void comb_sort(T* data, Index length) {
  Index gap = length;
  bool swapped = true;
  while (gap > 1 || swapped) {
    gap /= 1.3;
    if (gap < 1) gap = 1;
    swapped = false;
    for (Index index = 0; index < length - gap; index++) {
      if (data[index] > data[index + gap]) {
        T a = data[index];
        data[index] = data[index + gap];
        data[index + gap] = a;
        swapped = true
      }
    }
  }
}
