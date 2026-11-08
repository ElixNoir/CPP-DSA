#pragma once

#include "compare.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static void comb_sort(T* const begin, T* const end)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			size_t gap = (end - begin) + 1;
			bool swapped = true;
			while ((gap > 1) | swapped) {
				gap /= 1.3;
				if (gap == 0)
					gap = 1;
				swapped = false;
				for (T* cursor = begin; cursor < end - gap; cursor++) {
					if (compare<ASCENDING>(*cursor, *(cursor + gap))) {
						swap(*cursor, *(cursor + gap));
						swapped = true;
					}
				}
			}
		}

	}

}