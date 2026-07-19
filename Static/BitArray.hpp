#pragma once

#include <bit>
#include <cstdint>

struct BitArray {

  uint64_t Data;

  void flip(uint8_t index) {
    Data ^= 1 << index;
  }

  void flip(uint8_t start, uint64_t mask) {
    Data ^= mask << start;
  }

  void flip_length(uint8_t start, uint8_t length) {
    Data ^= (1 << length - 1) << start;
  }

  bool get(uint8_t index) const {
    return Data & (1 << index);
  }

  uint64_t get(uint8_t start, uint64_t mask) const {
    return Data & (mask << start);
  }

  uint64_t get(uint8_t start, uint8_t length) const {
    return Data & ((1 << length - 1) << start);
  }

  void keep(uint8_t index) {
    Data &= 1 << index;
  }

  void keep(uint8_t start, uint64_t mask) {
    Data &= mask << index;
  }

  void keep_length(uint8_t start, uint8_t length) {
    Data &= (1 << length - 1) << index;
  }

  void reset(uint8_t index) {
    Data &= ~(1 << index);
  }

  void reset(uint8_t start, uint64_t mask) {
    Data &= ~(mask << index);
  }

  void reset_length(uint8_t start, uint8_t length) {
    Data &= ~((1 << length - 1) << index);
  }

  void set(uint8_t index) {
    Data |= 1 << index;
  }

  void set(uint8_t start, uint64_t mask) {
    Data |= mask << index;
  }

  void set_length(uint8_t start, uint8_t length) {
    Data |= (1 << length - 1) << index;
  }

  uint8_t count_leading_ones() const {
    return std::countl_one();
  }

  uint8_t count_leading_zeros() const {
    return std::countl_zero();
  }

  uint8_t count_ones() const {
    return std::popcount(Data);
  }

  uint8_t count_zeros() const {
    return 8 * sizeof(Data) - std::popcount(Data);
  }

  uint8_t count_trailing_ones() const {
    return std::countr_one();
  }

  uint8_t count_trailing_zeros() const {
    return std::countr_zero();
  }

}
