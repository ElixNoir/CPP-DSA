#pragma once

#include <algorithm>

#include "Comparable.hpp"

namespace Algorithms::Sorting {

	template <bool ASCENDING = true, DSA::concepts::Comparable T>
		requires std::is_swappable_v<T>
	constexpr static void insertion_sort(T* const begin, const T* const end)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		for (T* cursor = begin + 1; cursor < end; ++cursor) {
			T* other = cursor;
			T temporary = std::move(*cursor);
			while (other > begin && compare<ASCENDING>(temporary, *(other - 1))) {
				*other = std::move(*(other - 1));
				--other;
			}
			*other = std::move(temporary);
		}
	}

}
