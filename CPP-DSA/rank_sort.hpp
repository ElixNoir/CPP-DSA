#pragma once

#include "compare.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static void rank_sort(T* const begin, T* const end)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			for (T* cursor = begin; cursor != end; cursor++) {
				T* rank = begin;

				for (T* other = begin; other != end; other++)
					rank += compare<ASCENDING>(*other, *cursor);

				swap(*cursor, *rank);
			}
		}

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static void rank_sortdup(T* const begin, T* const end)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			for (T* cursor = begin; cursor != end; cursor++) {
				T& data = *cursor;
				while (true) {
					T* rank = begin;
					
					for (T* other = begin; other != end; other++) {
						const T& otherValue = *other;
						if (compare<ASCENDING>(otherValue, data))
							rank++;
						else if ((otherValue == data) & (other < cursor))
							rank++;
					}

					if (rank == cursor)
						break;

					swap(data, *rank);
				}
			}
		}

	}

}