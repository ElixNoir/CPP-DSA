#pragma once

#pragma region Dependencies

#include <bit>
#include <concepts>
#include <cstdint>

#pragma endregion

template <std::unsigned_integral T = uintmax_t>
struct StaticBitArray {

  T Data;
  constexpr T Bits = 8 * sizeof(Data);
  constexpr T Maximum = ~(0);

#pragma region Methods

#pragma region Mask

  [[nodiscard]] constexpr static T mask(T index) const noexcept {
    return T{1} << index;
  }

  [[nodiscard]] constexpr static T mask(T start, T mask) const noexcept {
    return mask << start;
  }

  [[nodiscard]] constexpr static T mask_length(T start, T length) const noexcept {
    return ((T{1} << length) - T{1}) << start;
  }

  [[nodiscard]] constexpr static T mask_range(T start, T end) const noexcept {
    return mask_length(start, end - start);
  }

  [[nodiscard]] constexpr static T mask_range_inclusive(T start, T end) const noexcept {
    return ((T{1} << (end - start + 1)) - T{1}) << start;
  }

#pragma endregion

#pragma region Flip

  void flip() noexcept {
    Data ^= Maximum;
  }

  void flip(T index) noexcept {
    Data ^= mask(index);
  }

  void flip(T start, T mask) noexcept {
    Data ^= this->mask(start, mask);
  }

  void flip_length(T start, T length) noexcept {
    Data ^= mask_length(start, length);
  }

  void flip_range(T start, T end) noexcept {
    flip_length(start, end - start);
  }

  void flip_range_inclusive(T start, T end) noexcept {
    Data ^= mask_range_inclusive(start, end);
  }

  void flip_rightmost() noexcept {
    Data ^= get_rightmost();
  }

#pragma endregion

#pragma region Get

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

  void keep(T index) noexcept {
    Data = get(index);
  }

  void keep(T start, T mask) noexcept {
    Data = get(start, mask);
  }

  void keep_length(T start, T length) noexcept {
    Data = get_length(start, length);
  }

  void keep_range(T start, T end) noexcept {
    keep_length(start, end - start);
  }

  void keep_range_inclusive(T start, T end) noexcept {
    Data = get_range_inclusive(start, end);
  }

  void keep_rightmost() noexcept {
    Data = get_rightmost();
  }

#pragma endregion

#pragma region Reset

  void reset(T index) noexcept {
    Data &= ~(mask(index));
  }

  void reset(T start, T mask) noexcept {
    Data &= ~(this->mask(start, mask));
  }

  void reset_length(T start, T length) noexcept {
    Data &= ~(mask_length(start, length));
  }

  void reset_range(T start, T end) noexcept {
    reset_length(start, end - start);
  }

  void reset_range_inclusive(T start, T end) noexcept {
    Data &= ~(mask_range_inclusive(start, end));
  }

  void reset_rightmost() noexcept {
    Data &= Data - 1;
  }

#pragma endregion

#pragma region Set

  void set() noexcept {
    Data = Maximum;
  }

  void set(T index) noexcept {
    Data |= mask(index);
  }

  void set(T start, T mask) noexcept {
    Data |= this->mask(start, mask);
  }

  void set_length(T start, T length) noexcept {
    Data |= mask_length(start, length);
  }

  void set_range(T start, T end) noexcept {
    set_length(start, end - start);
  }

  void set_range_inclusive(T start, T end) noexcept {
    Data |= mask_range_inclusive(start, end);
  }

  void set_rightmost() noexcept {
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

}
