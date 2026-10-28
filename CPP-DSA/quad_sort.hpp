#pragma once

#include "Container.hpp"
#include "insertion_sort.hpp"
#include "Math.hpp"
#include "Sorting.hpp"

namespace Algorithms {

	namespace Sorting {

		template <size_t LENGTH, bool ASCENDING = true, DSA::concepts::Comparable T>
			requires (LENGTH > 1)
		constexpr static void quad_sort(T* const data) {
			using INDEX = smallest_uint_t<LENGTH>;

			INDEX index = 0;
			for (; index + 4 <= LENGTH; index += 4)
				Algorithms::Sorting::sort4<ASCENDING>(data + index);

			if (index < LENGTH)
				Algorithms::Sorting::insertion_sort<ASCENDING>(data + index, data + LENGTH);

			typename DSA::StaticContainerConfiguration<LENGTH>::template CONTAINER<T> temporaryData;

			INDEX block = 4;

			while (block < LENGTH) {
				const INDEX step = block * 2;
				INDEX pos = 0;

				while (pos < LENGTH) {
					const INDEX mid = Algorithms::Math::minimum<INDEX>(pos + block, LENGTH);
					const INDEX end = Algorithms::Math::minimum<INDEX>(pos + step, LENGTH);

					T* const left = data + pos;
					T* const right = data + mid;
					T* out = temporaryData.get_data() + pos;

					const INDEX leftLength = mid - pos;
					const INDEX rightLength = end - mid;

					INDEX i = 0, j = 0, k = 0;

					while (i < leftLength && j < rightLength) {
						if constexpr (ASCENDING) {
							if (right[j] < left[i])
								Memory::move_construct_at(out + k++, std::move(right[j++]));
							else
								Memory::move_construct_at(out + k++, std::move(left[i++]));
						}
						else {
							if (right[j] > left[i])
								Memory::move_construct_at(out + k++, std::move(right[j++]));
							else
								Memory::move_construct_at(out + k++, std::move(left[i++]));
						}
					}

					out += k;
					Memory::move_construct_range_forward(out, out + leftLength - i, left + i);
					out += leftLength - i;
					Memory::move_construct_range_forward(out, out + rightLength - j, right + j);

					pos = end;
				}

				Memory::move_construct_range_forward(data, data + LENGTH, temporaryData.get_data());

				block = step;
			}
		}

		template <DSA::concepts::Comparable T, bool ASCENDING = true, std::unsigned_integral INDEX = size_t>
		constexpr static void quad_sort(T* const data, INDEX length) {
			if (length < 2)
				return;

			INDEX index = 0;
			for (; index + 4 <= length; index += 4)
				Algorithms::Sorting::sort4<ASCENDING>(data + index);

			if (index < length)
				Algorithms::Sorting::insertion_sort<ASCENDING>(data + index, data + length);

			DSA::AlternateDynamicContainerConfiguration<>::CONTAINER<T> temporaryData(length);

			INDEX block = 4;

			while (block < length) {
				const INDEX step = block * 2;
				INDEX pos = 0;

				while (pos < length) {
					const INDEX mid = Algorithms::Math::minimum(pos + block, length);
					const INDEX end = Algorithms::Math::minimum(pos + step, length);

					T* const left = data + pos;
					T* const right = data + mid;
					T* out = temporaryData.get_data() + pos;

					const INDEX leftLength = mid - pos;
					const INDEX rightLength = end - mid;

					INDEX i = 0, j = 0, k = 0;

					while (i < leftLength && j < rightLength) {
						if constexpr (ASCENDING) {
							if (right[j] < left[i])
								Memory::move_construct_at(out + k++, std::move(right[j++]));
							else
								Memory::move_construct_at(out + k++, std::move(left[i++]));
						}
						else {
							if (right[j] > left[i])
								Memory::move_construct_at(out + k++, std::move(right[j++]));
							else
								Memory::move_construct_at(out + k++, std::move(left[i++]));
						}
					}

					out += k;
					Memory::move_construct_range_forward(out, out + leftLength - i, left + i);
					out += leftLength - i;
					Memory::move_construct_range_forward(out, out + rightLength - j, right + j);

					pos = end;
				}

				Memory::move_construct_range_forward(data, data + length, temporaryData.get_data());

				block = step;
			}
		}

	}

}