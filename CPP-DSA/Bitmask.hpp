#pragma once

#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>

template <std::unsigned_integral T = uintmax_t>
struct Bitmask {

#pragma region Properties

    T data;

#pragma endregion

#pragma region Helpers

    constexpr static T BitCount = std::numeric_limits<T>::digits;
	constexpr static T BitShift = std::bit_width(BitCount - 1);

    constexpr static T MaximumValue = std::numeric_limits<T>::max();
    constexpr static T MinimumValue = std::numeric_limits<T>::min();

    constexpr static T MaximumBit = T{ 1 } << (std::numeric_limits<T>::digits - 1);

#pragma endregion

#pragma region Methods

#pragma region Mask

    [[nodiscard]] constexpr static T mask_leading(T index) noexcept {
        return MaximumBit >> index;
    }

    [[nodiscard]] constexpr static T mask_leading(T start, T mask) noexcept {
        return mask << start;
    }

    [[nodiscard]] constexpr static T mask_leading_length(T start, T length) noexcept {
        return ((MaximumBit >> length) - T{ 1 }) >> start;
    }

    [[nodiscard]] constexpr static T mask_leading_range(T start, T end) noexcept {
        return mask_leading_length(start, end - start);
    }

    [[nodiscard]] constexpr static T mask_leading_range_inclusive(T start, T end) noexcept {
        return ((MaximumBit >> (end - start + T{ 1 })) - T{ 1 }) >> start;
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
        data ^= MaximumValue;
    }

    constexpr void flip(T index) noexcept {
        data ^= mask_trailing(index);
    }

    constexpr void flip(T start, T mask) noexcept {
        data ^= this->mask_trailing(start, mask);
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

    constexpr void flip_rightmost() noexcept {
        data ^= get_rightmost();
    }

#pragma endregion

#pragma region Get

    [[nodiscard]] constexpr T get() const noexcept {
        return data;
    }

    [[nodiscard]] constexpr bool get(T index) const noexcept {
        return data & mask_trailing(index);
    }

    [[nodiscard]] constexpr T get(T start, T mask) const noexcept {
        return data & this->mask_trailing(start, mask);
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

    [[nodiscard]] constexpr T get_rightmost() {
        return data & -data;
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

    constexpr void keep_rightmost() noexcept {
        data = get_rightmost();
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

    constexpr void reset_rightmost() noexcept {
        data &= data - 1;
    }

#pragma endregion

#pragma region Set

    constexpr void set() noexcept {
        data = MaximumValue;
    }

    constexpr void set(T index) noexcept {
        data |= mask_trailing(index);
    }

    constexpr void set(T start, T mask) noexcept {
        data |= this->mask_trailing(start, mask);
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

    constexpr void set_rightmost() noexcept {
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
        return BitCount - std::popcount(data);
    }

#pragma endregion

#pragma region Indexing

    [[nodiscard]] constexpr int index_of_leading_one() const noexcept {
        return BitCount - count_leading_zeros();
    }

    [[nodiscard]] constexpr int index_of_leading_one(T bitIndex) const noexcept {
        return BitCount - std::countl_zero(data & (MaximumValue >> bitIndex));
    }

    [[nodiscard]] constexpr int index_of_leading_zero() const noexcept {
        return BitCount - count_leading_ones();
    }

    [[nodiscard]] constexpr int index_of_leading_zero(T bitIndex) const noexcept {
        return BitCount - std::countl_zero(~data & (MaximumValue >> bitIndex));
    }

    [[nodiscard]] constexpr int index_of_trailing_one() const noexcept {
        return count_trailing_zeros();
    }

    [[nodiscard]] constexpr int index_of_trailing_one(T bitIndex) const noexcept {
        return std::countr_zero(data & (MaximumValue << bitIndex));
    }

    [[nodiscard]] constexpr int index_of_trailing_zero() const noexcept {
        return count_trailing_ones();
    }

    [[nodiscard]] constexpr int index_of_trailing_zero(T bitIndex) const noexcept {
        return std::countr_zero(~data & (MaximumValue << bitIndex));
    }

#pragma endregion

#pragma region Miscellaneous

    [[nodiscard]] constexpr bool is_maximum() const noexcept {
        return data == MaximumValue;
    }

    [[nodiscard]] constexpr bool is_minimum() const noexcept {
        return data == 0;
    }

    [[nodiscard]] constexpr bool is_power_of_two() const noexcept {
        return count_ones();
    }

#pragma endregion

#pragma endregion

};