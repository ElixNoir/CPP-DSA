#pragma once

#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>

namespace DSA {

	template <std::unsigned_integral _T = uintmax_t>
	struct Bitmask {

		using T = _T;

#pragma region Properties

		T data;

#pragma endregion

#pragma region Helpers

		constexpr static T bit_count = std::numeric_limits<T>::digits;
		constexpr static T bit_shift = std::bit_width(bit_count - 1);

		constexpr static T Evens = ~T{ 0 } / 3;
		constexpr static T Odds = Evens >> 1;

		constexpr static T maximum_value = std::numeric_limits<T>::max();
		constexpr static T minimum_value = std::numeric_limits<T>::min();

		constexpr static T maximum_bit = ~(maximum_value >> 1);

#pragma endregion

#pragma region Methods

#pragma region Mask

		[[nodiscard]] constexpr static T mask_leading(T index) noexcept {
			return maximum_bit >> index;
		}

		[[nodiscard]] constexpr static T mask_leading(T start, T mask) noexcept {
			return mask << start;
		}

		[[nodiscard]] constexpr static T mask_leading_length(T start, T length) noexcept {
			return ((maximum_bit >> length) - T{ 1 }) >> start;
		}

		[[nodiscard]] constexpr static T mask_leading_range(T start, T end) noexcept {
			return mask_leading_length(start, end - start);
		}

		[[nodiscard]] constexpr static T mask_leading_range_inclusive(T start, T end) noexcept {
			return ((maximum_bit >> (end - start + T{ 1 })) - T{ 1 }) >> start;
		}

		[[nodiscard]] constexpr static T mask_trailing(T index) noexcept {
			return T{ 1 } << index;
		}

		[[nodiscard]] constexpr static T mask_trailing(T start, T mask) noexcept {
			return mask << start;
		}

		[[nodiscard]] constexpr static T mask_trailing_length(T start, T length) noexcept {
			return ((T{ 1 } << length) - T{ 1 }) << start;
		}

		[[nodiscard]] constexpr static T mask_trailing_range(T start, T end) noexcept {
			return mask_trailing_length(start, end - start);
		}

		[[nodiscard]] constexpr static T mask_trailing_range_inclusive(T start, T end) noexcept {
			return ((T{ 1 } << (end - start + 1)) - T{ 1 }) << start;
		}

#pragma endregion

#pragma region Flip

		constexpr void flip() noexcept {
			data ^= maximum_value;
		}

		constexpr void flip(T index) noexcept {
			data ^= mask_trailing(index);
		}

		constexpr void flip(T start, T mask) noexcept {
			data ^= mask_trailing(start, mask);
		}

		constexpr void flip_length(T start, T length) noexcept {
			data ^= mask_trailing_length(start, length);
		}

		constexpr void flip_range(T start, T end) noexcept {
			flip_length(start, end - start);
		}

		constexpr void flip_range_inclusive(T start, T end) noexcept {
			data ^= mask_trailing_range_inclusive(start, end);
		}

		constexpr void flip_trailing() noexcept {
			data ^= get_trailing();
		}

#pragma endregion

#pragma region Get

		[[nodiscard]] constexpr T get() const noexcept {
			return data;
		}

		[[nodiscard]] constexpr T get(T index) const noexcept {
			return data & mask_trailing(index);
		}

		[[nodiscard]] constexpr T get(T start, T mask) const noexcept {
			return data & mask_trailing(start, mask);
		}

		[[nodiscard]] constexpr T get_leading() const noexcept {
			return data & maximum_bit;
		}

		[[nodiscard]] constexpr T get_length(T start, T length) const noexcept {
			return data & mask_trailing_length(start, length);
		}

		[[nodiscard]] constexpr T get_range(T start, T end) const noexcept {
			return get_length(start, end - start);
		}

		[[nodiscard]] constexpr T get_range_inclusive(T start, T end) const noexcept {
			return data & mask_trailing_range_inclusive(start, end);
		}

		[[nodiscard]] constexpr T get_trailing() const noexcept {
			return data & -data;
		}

		[[nodiscard]] constexpr T get_trailing_zero() const noexcept {
			return ~data & (data - 1);
		}

#pragma endregion

#pragma region Keep

		constexpr void keep(T index) noexcept {
			data = get(index);
		}

		constexpr void keep(T start, T mask) noexcept {
			data = get(start, mask);
		}

		constexpr void keep_length(T start, T length) noexcept {
			data = get_length(start, length);
		}

		constexpr void keep_range(T start, T end) noexcept {
			keep_length(start, end - start);
		}

		constexpr void keep_range_inclusive(T start, T end) noexcept {
			data = get_range_inclusive(start, end);
		}

		constexpr void keep_trailing() noexcept {
			data = get_trailing();
		}

#pragma endregion

#pragma region Reset

		constexpr void reset(T index) noexcept {
			data &= ~(mask_trailing(index));
		}

		constexpr void reset(T start, T mask) noexcept {
			data &= ~(this->mask_trailing(start, mask));
		}

		constexpr void reset_length(T start, T length) noexcept {
			data &= ~(mask_trailing_length(start, length));
		}

		constexpr void reset_range(T start, T end) noexcept {
			reset_length(start, end - start);
		}

		constexpr void reset_range_inclusive(T start, T end) noexcept {
			data &= ~(mask_trailing_range_inclusive(start, end));
		}

		constexpr void reset_trailing_zero() noexcept {
			data &= data - 1;
		}

#pragma endregion

#pragma region Set

		constexpr void set() noexcept {
			data = maximum_value;
		}

		constexpr void set(T index) noexcept {
			data |= mask_trailing(index);
		}

		constexpr void set(T start, T mask) noexcept {
			data |= mask_trailing(start, mask);
		}

		constexpr void set_length(T start, T length) noexcept {
			data |= mask_trailing_length(start, length);
		}

		constexpr void set_range(T start, T end) noexcept {
			set_length(start, end - start);
		}

		constexpr void set_range_inclusive(T start, T end) noexcept {
			data |= mask_trailing_range_inclusive(start, end);
		}

		constexpr void set_trailing_zero() noexcept {
			data |= data + 1;
		}

#pragma endregion

#pragma region Counting

		[[nodiscard]] constexpr int count_leading_ones() const noexcept {
			return std::countl_one(data);
		}

		[[nodiscard]] constexpr int count_leading_zeros() const noexcept {
			return std::countl_zero(data);
		}

		[[nodiscard]] constexpr int count_ones() const noexcept {
			return std::popcount(data);
		}

		[[nodiscard]] constexpr int count_trailing_ones() const noexcept {
			return std::countr_one(data);
		}

		[[nodiscard]] constexpr int count_trailing_zeros() const noexcept {
			return std::countr_zero(data);
		}

		[[nodiscard]] constexpr int count_zeros() const noexcept {
			return bit_count - std::popcount(data);
		}

#pragma endregion

#pragma region Indexing

		[[nodiscard]] constexpr int index_of_leading_one() const noexcept {
			return bit_count - count_leading_zeros();
		}

		[[nodiscard]] constexpr int index_of_leading_one(T bitIndex) const noexcept {
			return bit_count - std::countl_zero(data & (maximum_value >> bitIndex));
		}

		[[nodiscard]] constexpr int index_of_leading_zero() const noexcept {
			return bit_count - count_leading_ones();
		}

		[[nodiscard]] constexpr int index_of_leading_zero(T bitIndex) const noexcept {
			return bit_count - std::countl_zero(~data & (maximum_value >> bitIndex));
		}

		[[nodiscard]] constexpr int index_of_trailing_one() const noexcept {
			return count_trailing_zeros();
		}

		[[nodiscard]] constexpr int index_of_trailing_one(T bitIndex) const noexcept {
			return std::countr_zero(data & (maximum_value << bitIndex));
		}

		[[nodiscard]] constexpr int index_of_trailing_zero() const noexcept {
			return count_trailing_ones();
		}

		[[nodiscard]] constexpr int index_of_trailing_zero(T bitIndex) const noexcept {
			return std::countr_zero(~data & (maximum_value << bitIndex));
		}

#pragma endregion

#pragma region Miscellaneous

		[[nodiscard]] constexpr T compact_even_bits() noexcept {
			data &= Evens;
			data = (data | (data >> 1)) & (~T{ 0 } / 5);
			data = (data | (data >> 2)) & (~T{ 0 } / 17);
			data = (data | (data >> 4)) & (~T{ 0 } / 257);

			if constexpr (sizeof(T) == 8) {
				data = (data | (data >> 8)) & (~T{ 0 } / 65537);
				data = (data | (data >> 16)) & 0x00000000FFFFFFFFull;
			}

			return data;
		}

		[[nodiscard]] constexpr bool is_maximum() const noexcept {
			return data == maximum_value;
		}

		[[nodiscard]] constexpr bool is_minimum() const noexcept {
			return data == 0;
		}

		[[nodiscard]] constexpr bool is_power_of_2() const noexcept {
			return count_ones();
		}

#pragma endregion

#pragma endregion

	};

}