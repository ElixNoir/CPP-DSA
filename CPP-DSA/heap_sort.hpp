#pragma once

#include "compare.hpp"
#include "swap.hpp"

namespace Algorithms {

	namespace Sorting {

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static void heapify(T* const data, const size_t count, size_t root)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			size_t child = (root << 1) + 1;
			while (child < count) {
				if (child + 1 < count && compare<ASCENDING>(data[child], data[child + 1]))
					++child;
				T& a = data[root];
				T& b = data[child];
				if (compare<ASCENDING>(a, b)) {
					swap(a, b);
					root = child;
					child = (root << 1) + 1;
				}
				else
					break;
			}
		}

		template <bool ASCENDING = true, DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static void heap_sort(T* const data, const size_t count)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			for (size_t start = count >> 1; start > 0;)
				heapify<ASCENDING>(data, count, --start);

			for (size_t index = count - 1; index > 0; --index) {
				swap(data[0], data[index]);
				heapify<ASCENDING>(data, index, 0);
			}
		}

	}

}