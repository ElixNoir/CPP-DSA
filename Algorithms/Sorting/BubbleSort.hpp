#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#include <concepts>

#pragma endregion

template <Comparable T, std::unsigned_integral Index = uint32_t>
void bubble_sort(T* data, Index length) {
    bool swapped;
    do {
        swapped = false;
        Index jndex = 1;
        for (Index index = 0; index < length; index++) {
            if (data[index] > data[jndex]) {
                T a = data[index];
                data[index] = data[jndex];
                data[jndex] = a;
                swapped = true;
            }
            jndex++;
        }
        length--;
    } while (swapped);
}
