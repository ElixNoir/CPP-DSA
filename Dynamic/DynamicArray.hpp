#pragma once

#pragma region Dependencies

#include "DynamicAoSContainer.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicArray : public DynamicAoSContainer<T, Index, A> {
public:

  using Base = DynamicAoSContainer<T, Index, A>;

  DynamicArray(Index initialCapacity) : Base(initialCapacity) {}

  DynamicArray(DynamicArray& other) : Base(other) {}
  
  DynamicArray(DynamicArray&& other) : Base(other) {}

#pragma region Methods

#pragma region Operators

  [[nodiscard]] T& operator[](Index index) {
    return Data[index];
  }

  [[nodiscard]] const T& operator[](Index index) const {
    return Data[index];
  }

#pragma endregion

  // Iterators?  Etc?

#pragma endregion

};
