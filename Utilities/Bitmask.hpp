#pragma once

#pragma region Dependencies

#include <bit>
#include <concepts>
#include <cstdint>

#pragma endregion

template <std::unsigned_integral T = uintmax_t>
struct Bitmask {

#pragma region Properties

    T Data;

#pragma endregion

#pragma region Helpers

    constexpr static T Bits = 8 * sizeof(Data);
    constexpr static T Maximum = ~(0);

#pragma endregion

#pragma region Methods

#pragma region Mask

    [[nodiscard]] constexpr static T mask(T index) noexcept {
        return T{ 1 } << index;
    }

    [[nodiscard]] constexpr static T mask(T start, T mask) noexcept {
        return mask << start;
    }

    [[nodiscard]] constexpr static T mask_length(T start, T length) noexcept {
        return ((T{ 1 } << length) - T{ 1 }) << start;
    }

    [[nodiscard]] constexpr static T mask_range(T start, T end) noexcept {
        return mask_length(start, end - start);
    }

    [[nodiscard]] constexpr static T mask_range_inclusive(T start, T end) noexcept {
        return ((T{ 1 } << (end - start + 1)) - T{ 1 }) << start;
    }

#pragma endregion

#pragma region Flip

    constexpr void flip() noexcept {
        Data ^= Maximum;
    }

    constexpr void flip(T index) noexcept {
        Data ^= mask(index);
    }

    constexpr void flip(T start, T mask) noexcept {
        Data ^= this->mask(start, mask);
    }

    constexpr void flip_length(T start, T length) noexcept {
        Data ^= mask_length(start, length);
    }

    constexpr void flip_range(T start, T end) noexcept {
        flip_length(start, end - start);
    }

    constexpr void flip_range_inclusive(T start, T end) noexcept {
        Data ^= mask_range_inclusive(start, end);
    }

    constexpr void flip_rightmost() noexcept {
        Data ^= get_rightmost();
    }

#pragma endregion

#pragma region Get

    [[nodiscard]] constexpr T get() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr bool get(T index) const noexcept {
        return Data & mask(index);
    }

    [[nodiscard]] constexpr T get(T start, T mask) const noexcept {
        return Data & this->mask(start, mask);
    }

    [[nodiscard]] constexpr T get_length(T start, T length) const noexcept {
        return Data & mask_length(start, length);
    }

    [[nodiscard]] constexpr T get_range(T start, T end) const noexcept {
        return get_length(start, end - start);
    }

    [[nodiscard]] constexpr T get_range_inclusive(T start, T end) const noexcept {
        return Data & mask_range_inclusive(start, end);
    }

    [[nodiscard]] constexpr T get_rightmost() {
        return Data & -Data;
    }

#pragma endregion

#pragma region Keep

    constexpr void keep(T index) noexcept {
        Data = get(index);
    }

    constexpr void keep(T start, T mask) noexcept {
        Data = get(start, mask);
    }

    constexpr void keep_length(T start, T length) noexcept {
        Data = get_length(start, length);
    }

    constexpr void keep_range(T start, T end) noexcept {
        keep_length(start, end - start);
    }

    constexpr void keep_range_inclusive(T start, T end) noexcept {
        Data = get_range_inclusive(start, end);
    }

    constexpr void keep_rightmost() noexcept {
        Data = get_rightmost();
    }

#pragma endregion

#pragma region Reset

    constexpr void reset(T index) noexcept {
        Data &= ~(mask(index));
    }

    constexpr void reset(T start, T mask) noexcept {
        Data &= ~(this->mask(start, mask));
    }

    constexpr void reset_length(T start, T length) noexcept {
        Data &= ~(mask_length(start, length));
    }

    constexpr void reset_range(T start, T end) noexcept {
        reset_length(start, end - start);
    }

    constexpr void reset_range_inclusive(T start, T end) noexcept {
        Data &= ~(mask_range_inclusive(start, end));
    }

    constexpr void reset_rightmost() noexcept {
        Data &= Data - 1;
    }

#pragma endregion

#pragma region Set

    constexpr void set() noexcept {
        Data = Maximum;
    }

    constexpr void set(T index) noexcept {
        Data |= mask(index);
    }

    constexpr void set(T start, T mask) noexcept {
        Data |= this->mask(start, mask);
    }

    constexpr void set_length(T start, T length) noexcept {
        Data |= mask_length(start, length);
    }

    constexpr void set_range(T start, T end) noexcept {
        set_length(start, end - start);
    }

    constexpr void set_range_inclusive(T start, T end) noexcept {
        Data |= mask_range_inclusive(start, end);
    }

    constexpr void set_rightmost() noexcept {
        Data |= Data + 1;
    }

#pragma endregion

#pragma region Counting

    [[nodiscard]] constexpr int count_leading_ones() const noexcept {
        return std::countl_one(Data);
    }

    [[nodiscard]] constexpr int count_leading_zeros() const noexcept {
        return std::countl_zero(Data);
    }

    [[nodiscard]] constexpr int count_ones() const noexcept {
        return std::popcount(Data);
    }

    [[nodiscard]] constexpr int count_trailing_ones() const noexcept {
        return std::countr_one(Data);
    }

    [[nodiscard]] constexpr int count_trailing_zeros() const noexcept {
        return std::countr_zero(Data);
    }

    [[nodiscard]] constexpr int count_zeros() const noexcept {
        return Bits - std::popcount(Data);
    }

#pragma endregion

#pragma region Indexing

    [[nodiscard]] constexpr int index_of_leading_one() const noexcept {
        return Bits - count_leading_zeros();
    }

    [[nodiscard]] constexpr int index_of_leading_one(T bitIndex) const noexcept {
        return Bits - std::countl_zero(Data & (Maximum >> bitIndex));
    }

    [[nodiscard]] constexpr int index_of_leading_zero() const noexcept {
        return Bits - count_leading_ones();
    }

    [[nodiscard]] constexpr int index_of_leading_zero(T bitIndex) const noexcept {
        return Bits - std::countl_zero(~Data & (Maximum >> bitIndex));
    }

    [[nodiscard]] constexpr int index_of_trailing_one() const noexcept {
        return count_trailing_zeros();
    }

    [[nodiscard]] constexpr int index_of_trailing_one(T bitIndex) const noexcept {
        return std::countr_zero(Data & (Maximum << bitIndex));
    }

    [[nodiscard]] constexpr int index_of_trailing_zero() const noexcept {
        return count_trailing_ones();
    }

    [[nodiscard]] constexpr int index_of_trailing_zero(T bitIndex) const noexcept {
        return std::countr_zero(~Data & (Maximum << bitIndex));
    }

#pragma endregion

#pragma region Miscellaneous

    [[nodiscard]] constexpr bool is_maximum() const noexcept {
        return Data == Maximum;
    }

    [[nodiscard]] constexpr bool is_minimum() const noexcept {
        return Data == 0;
    }

    [[nodiscard]] constexpr bool is_power_of_two() const noexcept {
        return count_ones();
    }

#pragma endregion

#pragma endregion

};
