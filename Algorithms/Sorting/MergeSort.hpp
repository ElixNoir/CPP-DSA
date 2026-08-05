#pragma once

#pragma region Dependencies

#include "Comparable.hpp"

#include <cstdint>
#include <cstring>
#include <type_traits>

#pragma endregion

template <Comparable T, std::unsigned_integral Index = uint32_t>
void merge_sort(T* data, Index length) {
    if (length <= 1) return;

    T* source = data;
    T* destination = new T[length];

    Index width = 1;
    while (width < length) {
        Index doubleWidth = 2 * width;
        Index index = 0;
        while (index < length) {
            Index middle = index + width;
            middle = middle < length ? middle : length;
            
            Index right = index + doubleWidth;
            right = right < length ? right : length;

            Index sourceOffset = index;
            Index sourceOffset2 = middle;
            Index destinationOffset = index;
            
            while (sourceOffset < middle && sourceOffset2 < right) {
                if (source[sourceOffset] <= source[sourceOffset2])
                    destination[destinationOffset++] = source[sourceOffset++];
                else
                    destination[destinationOffset++] = source[sourceOffset2++];
            }

            if constexpr (std::is_trivially_copyable_v<T>) {
                std::memcpy(destination + destinationOffset,
                    source + sourceOffset,
                    (middle - sourceOffset) * sizeof(T));
                std::memcpy(destination + destinationOffset + middle - sourceOffset,
                    source + sourceOffset2,
                    (right - sourceOffset2) * sizeof(T));
                index += doubleWidth;
            } else {
                std::move(source + sourceOffset,
                    source + middle,
                    destination + destinationOffset);
                std::move(source + sourceOffset2,
                    source + right,
                    destination + destinationOffset + (middle - sourceOffset));
            }
        }

        T* temporary = source;
        source = destination;
        destination = temporary;
        
        width = doubleWidth;
    }

    if (source != data) {
        if constexpr (std::is_trivially_copyable_v<T>)
            std::memcpy(data, source, length * sizeof(T));
        else
            std::move(source, source + length, data);
    }
    
    delete[] destination;
}
