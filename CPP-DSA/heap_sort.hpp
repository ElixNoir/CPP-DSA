#pragma once

#include "Comparable.hpp"
#include "swap.hpp"

namespace Algorithms {

    namespace Sorting {

        template <DSA::concepts::Comparable T>
        constexpr static void heapify(T* data, size_t length, const size_t root) noexcept {
            size_t largest = root;
            size_t left = (root << 1) + 1;
            size_t right = (root << 1) + 2;

            if (left < length && data[left] > data[largest])
                largest = left;
            if (right < length && data[right] > data[largest])
                largest = right;
            if (largest != root) {
                swap(data[root], data[largest]);
                heapify(data, length, largest);
            }
        }

        template <DSA::concepts::Comparable T>
        constexpr static void heap_sort_recursive(T* data, size_t length) noexcept {
            for (size_t index = (length >> 1) - 1; index >= 0; --index)
                heapify(data, length, index);

            for (size_t index = length - 1; index >= 0; --index) {
                swap(data[0], data[index]);
                heapify(data, index, 0);
            }
        }

        template <bool ASCENDING = true, DSA::concepts::Comparable T>
        constexpr static void heap_sort(T* data, const size_t low, const size_t high) {
            size_t length = high - low + 1;

            for (size_t start = length >> 1; start > 0; --start) {
                size_t root = start;
                size_t child = (root << 1) + 1;
                while (child < length) {
                    if (child + 1 < length && compare<ASCENDING>(data[low + child], data[low + child + 1]))
                        ++child;
                    T& a = data[low + root];
                    T& b = data[low + child];
                    if (compare<ASCENDING>(a, b)) {
                        swap(a, b);
                        root = child;
                    }
                    else
                        break;
                }
            }

            for (size_t index = length - 1; index > 0; --index) {
                swap(data[low], data[low + index]);

                size_t root = 0;
                size_t child = (root << 1) + 1;
                while (child < index) {
                    if (child + 1 < index && compare<ASCENDING>(data[low + child], data[low + child + 1]))
                        ++child;
                    T& a = data[low + root];
                    T& b = data[low + child];
                    if (compare<ASCENDING>(a, b)) {
                        swap(a, b);
                        root = child;
                    }
                    else
                        break;
                }
            }
        }

    }

}