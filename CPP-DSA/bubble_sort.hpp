#pragma once

#include "Comparable.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <DSA::concepts::Comparable T>
		constexpr static void bubble_sort(T* const begin, T* end)
			noexcept(noexcept(swap(
				std::declval<T&>(),
				std::declval<T&>())))
		{
			bool swapped;
			do {
				swapped = false;
				T* j = begin + 1;
				for (T* cursor = begin; cursor < end; cursor++) {
					if (*cursor > *j) {
						swap(*cursor, *j);
						swapped = true;
					}
					j++;
				}
				end--;
			} while (swapped);
		}

		/*template <DSA::concepts::Comparable T, std::unsigned_integral Index = size_t>
		constexpr static void bubble_sort(T* data, Index length) {
			bool swapped;
			do {
				swapped = false;
				Index jndex = 1;
				for (Index index = 0; index < length; index++) {
					if (data[index] > data[jndex]) {
						swap(data[index], data[jndex]);
						swapped = true;
					}
					jndex++;
				}
				length--;
			} while (swapped);
		}*/

	}

}