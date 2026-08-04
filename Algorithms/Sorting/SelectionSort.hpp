#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#include <cstdint>

#pragma endregion

template <Comparable T, std::unsigned_integral Index = uint32_t>
void selection_sort(T* data, Index length) {
	for (Index index = 0; index < length; index++) {
		Index minimumIndex = index;

		for (Index jndex = index + 1; jndex < length; jndex++)
			if (data[jndex] < data[minimumIndex])
        minimumIndex = jndex;

		if (minimumIndex != index) {
      T temporary = data[index];
      data[index] = data[minimumIndex];
      data[minimumIndex] = temporary;
		}
	}
}
