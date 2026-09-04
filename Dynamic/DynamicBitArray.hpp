#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"
#include "StaticBitArray.hpp"

#pragma endregion

template <Allocator A = DefaultAllocator>
class DynamicBitArray {
protected:

  [[no_unique_address] A Alloc;

  StaticBitArray* Data;
  size_t Capacity;

  [[nodiscard]] constexpr StaticBitArray bit_array_at(size_t index) const noexcept {
    return Data[index >> std::bit_width(8 * sizeof(size_t) - 1)];
  }

  [[nodiscard]] constexpr size_t IndexBitMask = 8 * sizeof(size_t) - 1;

public:

  DynamicBitArray(size_t initialCapacity) {
    Data = static_cast<StaticBitArray*>(Alloc.allocate(initialCapacity * sizeof(StaticBitArray)));
  }

  ~DynamicBitArray() {
    Alloc.deallocate(Data);
  }

#pragma region Methods

#pragma region Getters

  [[nodiscard]] constexpr size_t capacity() const noexcept {
    return Capacity;
  }

  [[nodiscard]] constexpr StaticBitArray* data() const noexcept {
    return Data;
  }

#pragma endregion

#pragma region Memory Management

  void double_capacity() {
    grow(2 * Capacity);
  }

  void grow(size_t newCapacity) {
    if constexpr (ReallocatableAllocator<A>)
      Data = static_cast<StaticBitArray*>(Alloc.reallocate(Data, newCapacity));
    else {
      T* newData = static_cast<StaticBitArray*>(Alloc.allocate(newCapacity * sizeof(StaticBitArray)));
      std::memcpy(newData, Data, Capacity * sizeof(StaticBitArray));
      Alloc.deallocate(Data);
      Data = newData;
    }

    Capacity = newCapacity;
  }

  void reserve(size_t newCapacity) {
    if (newCapacity > Capacity) grow(newCapacity);
  }

  void resize(size_t newCapacity) {
    if (newCapacity < Capacity) shrink(newCapacity);
    else if (newCapacity > Capacity) grow(newCapacity);
  }

  void shrink(size_t newCapacity) {
    if constexpr (ReallocatableAllocator<A>)
      Data = static_cast<StaticBitArray*>(Alloc.reallocate(Data, newCapacity));
    else {
      T* newData = static_cast<StaticBitArray*>(Alloc.allocate(newCapacity * sizeof(StaticBitArray)));
      std::memcpy(newData, Data, newCapacity * sizeof(StaticBitArray));
      Alloc.deallocate(Data);
      Data = newData;
    }

    Capacity = newCapacity;
  }

#pragma endregion

#pragma region Flip

  void flip(size_t index) {
    bit_array_at(index).flip(index & IndexBitMask);
  }

  void flip(size_t start, uintmax_t mask) {
    bit_array_at(start).flip(start & IndexBitMask, mask);
  }

  void flip_length(size_t start, size_t length) {
    bit_array_at(start).flip_length(start & IndexBitMask, length);
  }

#pragma endregion

#pragma region Get

  bool get(size_t index) const {
    return bit_array_at(index).get(index & IndexBitMask);
  }

  uintmax_t get(size_t start, uintmax_t mask) const {
    return bit_array_at(start).get(start & IndexBitMask, mask);
  }

  uintmax_t get_length(size_t start, size_t length) const {
    return bit_array_at(start).get_length(start & IndexBitMask, length);
  }

#pragma endregion

#pragma region Keep

  void keep(size_t index) {
    bit_array_at(index).keep(index & IndexBitMask);
  }

  void keep(size_t start, uintmax_t mask) {
    bit_array_at(start).keep(start & IndexBitMask, mask);
  }

  void keep_length(size_t start, size_t length) {
    bit_array_at(start).keep_length(start & IndexBitMask, length);
  }

#pragma endregion

#pragma region Reset

  void reset(size_t index) {
    bit_array_at(index).reset(index & IndexBitMask);
  }

  void reset(size_t start, uintmax_t mask) {
    bit_array_at(start).reset(start & IndexBitMask, mask);
  }

  void reset_length(size_t start, size_t length) {
    bit_array_at(start).reset_length(start & IndexBitMask, length);
  }

#pragma endregion

#pragma region Set

  void set(size_t index) {
    bit_array_at(index).set(index & IndexBitMask);
  }

  void set(size_t start, uintmax_t mask) {
    bit_array_at(start).set(start & IndexBitMask, mask);
  }

  void set_length(size_t start, size_t length) {
    bit_array_at(start).set_length(start & IndexBitMask, length);
  }

#pragma endregion

#pragma region Counting

  size_t count_leading_ones(size_t index) const {
    return bit_array_at(index).count_leading_ones();
  }

  size_t count_leading_zeros(size_t index) const {
    return bit_array_at(index).count_leading_zeros();
  }

  size_t count_ones(size_t index) const {
    return bit_array_at(index).count_ones();
  }

  size_t count_trailing_ones(size_t index) const {
    return bit_array_at(index).count_trailing_ones();
  }

  size_t count_trailing_zeros(size_t index) const {
    return bit_array_at(index).count_trailing_zeros();
  }

  size_t count_zeros(size_t index) const {
    return bit_array_at(index).count_zeros();
  }

#pragma endregion

#pragma region Indexing

  size_t index_of_next_one(size_t index) const {
    return (index << 6) + index_of_next_one_in(index);
  }

  size_t index_of_next_one(size_t index, size_t bitIndex) const {
    return (index << 6) + index_of_next_one_in(index, bitIndex);
  }

  size_t index_of_next_one_in(size_t index) const {
    return bit_array_at(index).index_of_next_one();
  }

  size_t index_of_next_one_in(size_t index, size_t bitIndex) const {
    return bit_array_at(index).index_of_next_one(bitIndex);
  }

  size_t index_of_next_one(size_t index) const {
    return (index << 6) + index_of_next_zoro_in(index);
  }

  size_t index_of_next_zero(size_t index, size_t bitIndex) const {
    return (index << 6) + index_of_next_zero_in(index, bitIndex);
  }

  size_t index_of_next_zero_in(size_t index) const {
    return bit_array_at(index).index_of_next_zero();
  }

  size_t index_of_next_zero_in(size_t index, size_t bitIndex) const {
    return bit_array_at(index).index_of_next_zero(bitIndex);
  }

#pragma endregion

#pragma endregion

};
