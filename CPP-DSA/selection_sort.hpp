#pragma once

#include "Comparable.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
		void selection_sort(T* begin, const T* const end)
			noexcept(noexcept(swap(
				std::declval<T&>(),
				std::declval<T&>())))
		{
			for (; begin < end; begin++) {
				T* minimumPointer = begin;

				for (T* cursor = begin + 1; cursor < end; cursor++) {
					if constexpr (ASCENDING) {
						if (*cursor < *minimumPointer)
							minimumPointer = cursor;
					}
					else
						if (*cursor > *minimumPointer)
							minimumPointer = cursor;
				}

				if (minimumPointer != begin)
					swap(*begin, *minimumPointer)
			}
		}

	}

}