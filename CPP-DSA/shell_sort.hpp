#pragma once

#include "compare.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		void shell_sort(T* const begin, const T* const end)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			size_t gap = (end - begin) >> 1;
			while (gap > 0) {
				gap >>= 1;
				T* const gapPointer = begin + gap;
				for (T* cursor = gapPointer; cursor < end; cursor++) {
					T temporary = std::move(*cursor);
					while (cursor >= gapPointer && compare<ASCENDING>(temporary, *(cursor - gap))) {
						*cursor = std::move(*(cursor - gap));
						cursor -= gap;
					}
					*cursor = std::move(temporary);
				}
			}
		}

	}

}