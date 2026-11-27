#pragma once

#include "Bitmask.hpp"

#include <cstddef>
#include <iterator>

namespace DSA {

	template <std::unsigned_integral INDEX, bool ONES, std::unsigned_integral MASK_WORD = uintmax_t>
	class BitsetIterator {
	public:

		using MASK = Bitmask<MASK_WORD>;

#pragma region C++ Iterator Traits

		using iterator_category = std::forward_iterator_tag;
		using iterator_concept = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using pointer = void;
		using reference = INDEX;
		using value_type = INDEX;

#pragma endregion

	protected:

#pragma region Properties

		const MASK* address = nullptr;
		INDEX mask = 0;
		INDEX bit = MASK::bit_count;
		INDEX maskCount = 0;

#pragma endregion

#pragma region Methods

		constexpr void seek() {
			while (mask < maskCount) {
				const int found = ONES ?
					address[mask].index_of_trailing_one(static_cast<int>(bit))
					: address[mask].index_of_trailing_zero(static_cast<int>(bit));

				if (found != MASK::bit_count) {
					bit = static_cast<INDEX>(found);
					return;
				}

				++mask;
				bit = 0;
			}

			// End sentinel.
			mask = maskCount;
			bit = MASK::bit_count;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		constexpr BitsetIterator() = default;

		constexpr BitsetIterator(
			const MASK* address,
			INDEX maskCount,
			bool end
		) :
			address(address),
			mask(0),
			bit(MASK::bit_count),
			maskCount(maskCount)
		{
			if (!end)
				seek();
			else
				mask = maskCount;
		}

#pragma endregion

#pragma region Operators

		constexpr INDEX operator*() const noexcept {
			return mask * MASK::bit_count + bit;
		}

		constexpr BitsetIterator& operator++() {
			++bit;
			seek();
			return *this;
		}

		constexpr BitsetIterator operator++(int) {
			auto copy = *this;
			++*this;
			return copy;
		}

		constexpr bool operator==(
			const BitsetIterator& other
			) const noexcept {
			return address == other.address
				&& mask == other.mask
				&& bit == other.bit;
		}

		constexpr bool operator!=(
			const BitsetIterator& other
			) const noexcept {
			return !(*this == other);
		}

#pragma endregion

	};

}