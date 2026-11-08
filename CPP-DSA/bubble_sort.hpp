#pragma once

#include "compare.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static void bubble_sort(T* const begin, T* end)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			bool swapped;
			do {
				swapped = false;
				T* j = begin + 1;
				for (T* cursor = begin; cursor < end; ++cursor) {
					if (compare<ASCENDING>(*j, *cursor)) {
						swap(*cursor, *j);
						swapped = true;
					}
					++j;
				}
				--end;
			} while (swapped);
		}

	}

}