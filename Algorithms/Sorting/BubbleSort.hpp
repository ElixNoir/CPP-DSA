#pragma once

template <Comparable T>
void bubble_sort(T* data, size_t length) {
    bool swapped;
    do {
        swapped = false;
        size_t jndex = 1;
        for (size_t index = 0; index < length; index++) {
            T& a = data[index];
            T& b = data[jndex];
            if (a > b) {
                data[index] = b;
                data[jndex] = a;
                swapped = true;
            }
            jndex++;
        }
        length--;
    } while (swapped);
}
