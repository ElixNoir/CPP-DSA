#pragma once

#include <bit>
#include <cstdint>

struct StaticBitArray {

  uintmax_t Data;
  constexpr uintmax_t Bits = 8 * sizeof(Data);
  constexpr uintmax_t Maximum = ~(0);

#pragma region Methods

#pragma region Flip

  void flip() noexcept {
    Data ^= Maximum;
  }

  void flip(uintmax_t index) noexcept {
    Data ^= uintmax_t{1} << index;
  }

  void flip(uintmax_t start, uintmax_t mask) noexcept {
    Data ^= mask << start;
  }

  void flip_length(uintmax_t start, uintmax_t length) noexcept {
    Data ^= ((uintmax_t{1} << length) - uintmax_t{1}) << start;
  }

  void flip_range(uintmax_t start, uintmax_t end) noexcept {
    flip_length(start, end - start);
  }

  void flip_range_inclusive(uintmax_t start, uintmax_t end) noexcept {
    Data ^= ((uintmax_t{1} << (end - start + 1)) - uintmax_t{1}) << start;
  }

#pragma endregion

#pragma region Get

  [[nodiscard]] constexpr bool get(uintmax_t index) const noexcept {
    return Data & (uintmax_t{1} << index);
  }

  [[nodiscard]] constexpr uintmax_t get(uintmax_t start, uintmax_t mask) const noexcept {
    return Data & (mask << start);
  }

  [[nodiscard]] constexpr uintmax_t get_length(uintmax_t start, uintmax_t length) const noexcept {
    return Data & (((uintmax_t{1} << length) - uintmax_t{1}) << start);
  }

  [[nodiscard]] constexpr uintmax_t get_range(uintmax_t start, uintmax_t end) const noexcept {
    return get_length(start, end - start);
  }

  [[nodiscard]] constexpr uintmax_t get_range_inclusive(uintmax_t start, uintmax_t end) const noexcept {
    return Data & (((uintmax_t{1} << (end - start + 1)) - uintmax_t{1}) << start);
  }

  [[nodiscard]] constexpr uintmax_t get_rightmost() {
    return Data & -Data;
  }

#pragma endregion

#pragma region Keep

  void keep(uintmax_t index) noexcept {
    Data &= uintmax_t{1} << index;
  }

  void keep(uintmax_t start, uintmax_t mask) noexcept {
    Data &= mask << start;
  }

  void keep_length(uintmax_t start, uintmax_t length) noexcept {
    Data &= ((uintmax_t{1} << length) - uintmax_t{1}) << start;
  }

  void keep_range(uintmax_t start, uintmax_t end) noexcept {
    keep_length(start, end - start);
  }

  void keep_range_inclusive(uintmax_t start, uintmax_t end) noexcept {
    Data &= ((uintmax_t{1} << (end - start + 1)) - uintmax_t{1}) << start;
  }

#pragma endregion

#pragma region Reset

  void reset(uintmax_t index) noexcept {
    Data &= ~(uintmax_t{1} << index);
  }

  void reset(uintmax_t start, uintmax_t mask) noexcept {
    Data &= ~(mask << start);
  }

  void reset_length(uintmax_t start, uintmax_t length) noexcept {
    Data &= ~(((uintmax_t{1} << length) - uintmax_t{1}) << start);
  }

  void reset_range(uintmax_t start, uintmax_t end) noexcept {
    reset_length(start, end - start);
  }

  void reset_range_inclusive(uintmax_t start, uintmax_t end) noexcept {
    Data &= ~(((uintmax_t{1} << (end - start + 1)) - uintmax_t{1}) << start);
  }

  void reset_rightmost() noexcept {
    Data &= Data - 1;
  }

#pragma endregion

#pragma region Set

  void set() noexcept {
    Data = Maximum;
  }

  void set(uintmax_t index) noexcept {
    Data |= uintmax_t{1} << index;
  }

  void set(uintmax_t start, uintmax_t mask) noexcept {
    Data |= mask << start;
  }

  void set_length(uintmax_t start, uintmax_t length) noexcept {
    Data |= ((uintmax_t{1} << length) - uintmax_t{1}) << start;
  }

  void set_range(uintmax_t start, uintmax_t end) noexcept {
    set_length(start, end - start);
  }

  void set_range_inclusive(uintmax_t start, uintmax_t end) noexcept {
    Data |= ((uintmax_t{1} << (end - start + 1)) - uintmax_t{1}) << start;
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

  [[nodiscard]] constexpr int index_of_leading_one(uintmax_t bitIndex) const noexcept {
    return Bits - std::countl_zero(Data & (Maximum >> bitIndex));
  }

  [[nodiscard]] constexpr int index_of_leading_zero() const noexcept {
    return Bits - count_leading_ones();
  }

  [[nodiscard]] constexpr int index_of_leading_zero(uintmax_t bitIndex) const noexcept {
    return Bits - std::countl_zero(~Data & (Maximum >> bitIndex));
  }

  [[nodiscard]] constexpr int index_of_trailing_one() const noexcept {
    return count_trailing_zeros();
  }

  [[nodiscard]] constexpr int index_of_trailing_one(uintmax_t bitIndex) const noexcept {
    return std::countr_zero(Data & (Maximum << bitIndex));
  }

  [[nodiscard]] constexpr int index_of_trailing_zero() const noexcept {
    return count_trailing_ones();
  }

  [[nodiscard]] constexpr int index_of_trailing_zero(uintmax_t bitIndex) const noexcept {
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
