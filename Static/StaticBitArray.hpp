#pragma once

#include <bit>
#include <cstdint>

struct StaticBitArray {

  uintmax_t Data;

#pragma region Methods

#pragma region Flip

  void flip(uint8_t index) {
    Data ^= uintmax_t{1} << index;
  }

  void flip(uint8_t start, uintmax_t mask) {
    Data ^= mask << start;
  }

  void flip_length(uint8_t start, uint8_t length) {
    Data ^= (uintmax_t{1} << length - 1) << start;
  }

#pragma endregion

#pragma region Get

  bool get(uint8_t index) const {
    return Data & (uintmax_t{1} << index);
  }

  uintmax_t get(uint8_t start, uintmax_t mask) const {
    return Data & (mask << start);
  }

  uintmax_t get_length(uint8_t start, uint8_t length) const {
    return Data & ((uintmax_t{1} << length - 1) << start);
  }

#pragma endregion

#pragma region Keep

  void keep(uint8_t index) {
    Data &= uintmax_t{1} << index;
  }

  void keep(uint8_t start, uintmax_t mask) {
    Data &= mask << start;
  }

  void keep_length(uint8_t start, uint8_t length) {
    Data &= (uintmax_t{1} << length - 1) << start;
  }

#pragma endregion

#pragma region Reset

  void reset(uint8_t index) {
    Data &= ~(uintmax_t{1} << index);
  }

  void reset(uint8_t start, uintmax_t mask) {
    Data &= ~(mask << start);
  }

  void reset_length(uint8_t start, uint8_t length) {
    Data &= ~((uintmax_t{1} << length - 1) << start);
  }

#pragma endregion

#pragma region Set

  void set(uint8_t index) {
    Data |= uintmax_t{1} << index;
  }

  void set(uint8_t start, uintmax_t mask) {
    Data |= mask << start;
  }

  void set_length(uint8_t start, uint8_t length) {
    Data |= (uintmax_t{1} << length - 1) << start;
  }

#pragma endregion

#pragma region Counting

  uint8_t count_leading_ones() const {
    return std::countl_one(Data);
  }

  uint8_t count_leading_zeros() const {
    return std::countl_zero(Data);
  }

  uint8_t count_ones() const {
    return std::popcount(Data);
  }

  uint8_t count_trailing_ones() const {
    return std::countr_one(Data);
  }

  uint8_t count_trailing_zeros() const {
    return std::countr_zero(Data);
  }

  uint8_t count_zeros() const {
    return 8 * sizeof(Data) - std::popcount(Data);
  }

#pragma endregion

#pragma endregion

}
