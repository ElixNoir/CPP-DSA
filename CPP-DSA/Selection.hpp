#pragma once

#include "Comparable.hpp"
#include "random.hpp"
#include "swap.hpp"

namespace Algorithms {

    namespace Selection {

        template <DSA::concepts::Comparable T>
            requires std::is_swappable_v<T>
        constexpr static T quick_select(T* data, size_t left, size_t right, size_t k)
            noexcept (std::is_nothrow_swappable_v<T>)
        {
            const T* end = data + right;
            while (true) {
                if (left == right)
                    return data[left];

                size_t pivotIndex = random(left, right);
                T& pivotValue = data[pivotIndex];

                swap(pivotValue, data[right]);
                pivotIndex = left;
                for (T* cursor = data + left; cursor < end; ++cursor) {
                    if (*cursor < pivotValue) {
                        swap(*cursor, pivotValue);
                        ++pivotIndex;
                    }
                }
                swap(pivotValue, data[right]);

                if (k == pivotIndex)
                    return data[k];

                if (k < pivotIndex) {
                    right = pivotIndex - 1;
                    end = data + right;
                }
                else
                    left = pivotIndex + 1;
            }
        }

    }

}