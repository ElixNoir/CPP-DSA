#pragma once

#include "Comparable.hpp"

#include <cstdint>

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
		constexpr static void insertion_sort(T* const begin, const T* const end)
			noexcept(
				std::is_nothrow_move_constructible_v<T>
				&& std::is_nothrow_move_assignable_v<T>)
		{
			for (T* cursor = begin + 1; cursor < end; cursor++) {
				T* other = cursor;
				T temporary = std::move(*cursor);
				if constexpr (ASCENDING) {
					while (other > begin && *(other - 1) > temporary) {
						*other = std::move(*(other - 1));
						other--;
					}
				}
				else {
					while (other > begin && *(other - 1) < temporary) {
						*other = std::move(*(other - 1));
						other--;
					}
				}
				*other = std::move(temporary);
			}
		}

	}

}