#pragma once

#include "Bitmask.hpp"

namespace DSA {

	template <std::unsigned_integral _T = uintmax_t>
	class HierarchicalBitset2 {
	public:

		using INDEX = uint16_t;
		using T = _T;

		constexpr static INDEX bit_count = Bitmask<T>::bit_count;
		constexpr static INDEX bit_shift = Bitmask<T>::bit_shift;
		constexpr static INDEX invalid_index = bit_count << bit_shift;

	protected:

		Bitmask<T> root{};
		Bitmask<T> layer1[bit_count]{};

	public:

#pragma region Methods

#pragma region Contains, Find, Reset, & Set

		[[nodiscard]] constexpr INDEX contains(INDEX index) noexcept {
			return layer1[index >> bit_shift].get_bit_at(index & (bit_count - 1));
		}

		[[nodiscard]] constexpr INDEX find_next_one(INDEX start) const noexcept {
			const INDEX index = root.next_trailing_one(start >> bit_shift);
			const INDEX index1 = layer1[index].next_trailing_one();
			return (index << bit_shift) + index1;
		}

		constexpr void reset(INDEX index) noexcept {
			const INDEX convertedIndex = index >> bit_shift;
			Bitmask<>& layer1Bitmask = layer1[convertedIndex];
			layer1Bitmask.reset_bit_at(index & (bit_count - 1));
			if (layer1Bitmask.count_ones() == 0)
				root.reset_bit_at(convertedIndex);
		}

		constexpr void set(INDEX index) noexcept {
			const INDEX convertedIndex = index >> bit_shift;
			root.set_bit_at(convertedIndex);
			layer1[convertedIndex].set_bit_at(index & (bit_count - 1));
		}

#pragma endregion

#pragma endregion

	};

}