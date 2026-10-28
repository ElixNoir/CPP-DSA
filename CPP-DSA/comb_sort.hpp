#pragma once

#include "Comparable.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
		constexpr static void comb_sort(T* const begin, T* end)
			noexcept(noexcept(swap(
				std::declval<T&>(),
				std::declval<T&>())))
		{
			size_t gap = (end - begin);
			bool swapped = true;
			while ((gap > 1) | swapped) {
				gap /= 1.3;
				if (gap == 0)
					gap = 1;
				swapped = false;
				for (T* cursor = begin; cursor < end - gap; cursor++) {
					if constexpr (ASCENDING) {
						if (*cursor > *(cursor + gap)) {
							swap(*cursor, *(cursor + gap));
							swapped = true;
						}
					}
					else {
						if (*cursor < *(cursor + gap)) {
							swap(*cursor, *(cursor + gap));
							swapped = true;
						}
					}
				}
			}
		}

		/*template <DSA::concepts::Comparable T, std::unsigned_integral Index = size_t>
		constexpr static void comb_sort(T* data, Index length) {
			Index gap = length;
			bool swapped = true;
			while ((gap > 1) | swapped) {
				gap /= 1.3;
				if (gap == 0)
					gap = 1;
				swapped = false;
				for (Index index = 0; index < length - gap; index++) {
					if (data[index] > data[index + gap]) {
						swap(data[index], data[index + gap]);
						swapped = true;
					}
				}
			}
		}*/

	}

}