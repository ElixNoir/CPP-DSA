#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#include <cstdint>

#pragma endregion

template <Comparable T, std::unsigned_integral Index = uint32_t>
void shell_sort(T* data, Index length) {
  Index gap = length >> 1;
  while (gap > 0) {
    gap >>= 1;
    for (Index index = gap; index <= length; index++) {
      T temporary = data[index];
      while (index >= gap && data[index - gap] > temporary) {
        data[index] = data[index - gap];
        index -= gap;
      }
      data[index] = temporary;
    }
  }
}
