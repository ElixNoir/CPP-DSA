#pragma once

#include <bit>
#include <cstdint>

struct StaticBitArray {

  uintmax_t Data;
  constexpr uintmax_t Bits = 8 * sizeof(Data);
  constexpr uintmax_t Maximum = ~(0);

#pragma region Methods

#pragma region Flip

  void flip(uintmax_t index) {
    Data ^= uintmax_t{1} << index;
  }

  void flip(uintmax_t start, uintmax_t mask) {
    Data ^= mask << start;
  }

  void flip_length(uintmax_t start, uintmax_t length) {
    Data ^= (uintmax_t{1} << length - uintmax_t{1}) << start;
  }

#pragma endregion

#pragma region Get

  bool get(uintmax_t index) const {
    return Data & (uintmax_t{1} << index);
  }

  uintmax_t get(uintmax_t start, uintmax_t mask) const {
    return Data & (mask << start);
  }

  uintmax_t get_length(uintmax_t start, uintmax_t length) const {
    return Data & ((uintmax_t{1} << length - uintmax_t{1}) << start);
  }

#pragma endregion

#pragma region Keep

  void keep(uintmax_t index) {
    Data &= uintmax_t{1} << index;
  }

  void keep(uintmax_t start, uintmax_t mask) {
    Data &= mask << start;
  }

  void keep_length(uintmax_t start, uintmax_t length) {
    Data &= (uintmax_t{1} << length - uintmax_t{1}) << start;
  }

#pragma endregion

#pragma region Reset

  void reset(uintmax_t index) {
    Data &= ~(uintmax_t{1} << index);
  }

  void reset(uintmax_t start, uintmax_t mask) {
    Data &= ~(mask << start);
  }

  void reset_length(uintmax_t start, uintmax_t length) {
    Data &= ~((uintmax_t{1} << length - uintmax_t{1}) << start);
  }

#pragma endregion

#pragma region Set

  void set(uintmax_t index) {
    Data |= uintmax_t{1} << index;
  }

  void set(uintmax_t start, uintmax_t mask) {
    Data |= mask << start;
  }

  void set_length(uintmax_t start, uintmax_t length) {
    Data |= (uintmax_t{1} << length - uintmax_t{1}) << start;
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

#pragma endregion

}
