#pragma once

#include "Comparable.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
		void shell_sort(T* const begin, const T* const end)
			noexcept(
				std::is_nothrow_move_constructible_v<T>
				&& std::is_nothrow_move_assignable_v<T>)
		{
			size_t gap = (end - begin) >> 1;
			while (gap > 0) {
				gap >>= 1;
				T* const gapPointer = begin + gap;
				for (T* cursor = gapPointer; cursor < end; cursor++) {
					T temporary = std::move(*cursor);
					if constexpr (ASCENDING) {
						while (cursor >= gapPointer && *(cursor - gap) > temporary) {
							*cursor = std::move(*(cursor - gap));
							cursor -= gap;
						}
					}
					else {
						while (cursor >= gapPointer && *(cursor - gap) < temporary) {
							*cursor = std::move(*(cursor - gap));
							cursor -= gap;
						}
					}
					*cursor = std::move(temporary);
				}
			}
		}

		/*
		template <DSA::concepts::Comparable T, std::unsigned_integral Index = size_t>
		void shell_sort(T* const data, Index length) {
			Index gap = length >> 1;
			while (gap > 0) {
				gap >>= 1;
				for (Index index = gap; index < length; index++) {
					T temporary = std::move(data[index]);
					while (index >= gap && data[index - gap] > temporary) {
						data[index] = std::move(data[index - gap]);
						index -= gap;
					}
					data[index] = std::move(temporary);
				}
			}
		}
		*/

	}

}