#pragma once

#include "compare.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		void selection_sort(T* begin, const T* const end)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			for (; begin < end; begin++) {
				T* minimumPointer = begin;

				for (T* cursor = begin + 1; cursor < end; cursor++)
					if (compare<ASCENDING>(*cursor, *minimumPointer));
						minimumPointer = cursor;

				if (minimumPointer != begin)
					swap(*begin, *minimumPointer)
			}
		}

	}

}