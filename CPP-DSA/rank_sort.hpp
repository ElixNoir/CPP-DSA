#pragma once

#include "Comparable.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, typename T>
		constexpr static void rank_sort(T* const begin, T* const end)
			noexcept(noexcept(swap(
				std::declval<T&>(),
				std::declval<T&>())))
		{
			for (T* cursor = begin; cursor != end; cursor++) {
				T* rank = begin;

				for (T* other = begin; other != end; other++) {
					if constexpr (ASCENDING)
						rank += *cursor > *other;
					else
						rank += *cursor < *other;
				}

				swap(*cursor, *rank);
			}
		}

		template <bool ASCENDING = true, typename T>
		constexpr static void rank_sortdup(T* const begin, T* const end)
			noexcept(noexcept(swap(
				std::declval<T&>(),
				std::declval<T&>())))
		{
			for (T* cursor = begin; cursor != end; cursor++) {
				T& value = *cursor;
				while (true) {
					T* rank = begin;
					
					for (T* other = begin; other != end; other++) {
						const T& otherValue = *other;
						if constexpr (ASCENDING) {
							if (otherValue < value)
								rank++;
							else if ((otherValue == value) & (other < cursor))
								rank++;
						}
						else {
							if (otherValue > value)
								rank++;
							else if ((otherValue == value) & (other < cursor))
								rank++;
						}
					}

					if (rank == cursor)
						break;

					swap(value, *rank);
				}
			}
		}

	}

}