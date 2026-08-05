#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#include <cstdint>

#pragma endregion

template <Comparable T, std::unsigned_integral Index = uint32_t>
void insertion_sort(T* data, Index length) {
	for (Index index = 1; index < length; index++) {
		Index jndex = index;

    T temporary = data[index];
		while (jndex > 0 && data[jndex - 1] > temporary) {
			data[jndex] = data[jndex - 1]; 
			jndex--;
		}

		data[jndex] = temporary;
	}
}
