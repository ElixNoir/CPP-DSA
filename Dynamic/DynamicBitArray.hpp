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

#pragma region Access

  void flip(size_t index) {
    Data[index >> 6].flip(index & size_t(63));
  }

  void flip(size_t start, uint64_t mask) {
    Data[start >> 6].flip(start & size_t(63), mask);
  }

  void flip_length(size_t start, size_t length) {
    Data[start >> 6].flip_length(start & size_t(63), length);
  }

  bool get(size_t index) const {
    return Data[index >> 6].get(index & size_t(63));
  }

  uint64_t get(size_t start, uint64_t mask) const {
    return Data[start >> 6].get(start & size_t(63), mask);
  }

  uint64_t get_length(size_t start, size_t length) const {
    return Data[start >> 6].get_length(start & size_t(63), length);
  }

  void keep(size_t index) {
    Data[index >> 6].keep(index & size_t(63));
  }

  void keep(size_t start, uint64_t mask) {
    Data[start >> 6].keep(start & size_t(63), mask);
  }

  void keep_length(size_t start, size_t length) {
    Data[start >> 6].keep_length(start & size_t(63), length);
  }

  void reset(size_t index) {
    Data[index >> 6].reset(index & size_t(63));
  }

  void reset(size_t start, uint64_t mask) {
    Data[start >> 6].reset(start & size_t(63), mask);
  }

  void reset_length(size_t start, size_t length) {
    Data[start >> 6].reset_length(start & size_t(63), length);
  }

  void set(size_t index) {
    Data[index >> 6].set(index & size_t(63));
  }

  void set(size_t start, uint64_t mask) {
    Data[start >> 6].set(start & size_t(63), mask);
  }

  void set_length(size_t start, size_t length) {
    Data[start >> 6].set_length(start & size_t(63), length);
  }

  size_t count_leading_ones(size_t index) const {
    return Data[index >> 6].count_leading_ones();
  }

  size_t count_leading_zeros(size_t index) const {
    return Data[index >> 6].count_leading_zeros();
  }

  size_t count_ones(size_t index) const {
    return Data[index >> 6].count_ones();
  }

  size_t count_zeros(size_t index) const {
    return Data[index >> 6].count_zeros();
  }

  size_t count_trailing_ones(size_t index) const {
    return Data[index >> 6].count_trailing_ones();
  }

  size_t count_trailing_zeros(size_t index) const {
    return Data[index >> 6].count_trailing_zeros();
  }

#pragma endregion

#pragma endregion

};
