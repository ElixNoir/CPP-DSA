#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#include <cstdint>

#pragma endregion

template <Comparable T, std::unsigned_integral Index = uint32_t>
void hash_sort(T* data, Index length) {
  Index hysteresisCount = 0;
  Index swapCount = 0;
  
  while (swapCount != length && hysteresisCount != length) {
    T value = data[hysteresisCount];
    if (data[value] == value) hysteresisCount++;
    else {
      data[hysteresisCount] = data[value];
      data[value] = value;
      swapCount++;
    }
  }
}
