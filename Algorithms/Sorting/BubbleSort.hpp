#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#pragma endregion

template <Comparable T>
void bubble_sort(T* data, size_t length) {
    bool swapped;
    do {
        swapped = false;
        size_t jndex = 1;
        for (size_t index = 0; index < length; index++) {
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
