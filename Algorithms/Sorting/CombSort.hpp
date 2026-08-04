#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#pragma endregion

template <Comparable T>
void comb_sort(T* data, size_t length) {
  size_t gap = length;
  bool swapped = true;
  while (gap > 1 || swapped) {
    gap /= 1.3;
    if (gap < 1) gap = 1;
    swapped = false;
    for (size_t index = 0; index < length - gap; index++) {
      if (data[index] > data[index + gap]) {
        T a = data[index];
        data[index] = data[index + gap];
        data[index + gap] = a;
        swapped = true
      }
    }
  }
}
