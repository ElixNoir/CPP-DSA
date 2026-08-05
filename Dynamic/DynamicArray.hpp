#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>
#include <cstring>
#include <type_traits>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
  requires (std::is_trivially_copyable_v<T>)
class DynamicArray {
protected:

  [[no_unique_address]] A Alloc;

  T* Data;
  Index Capacity;

public:

  DynamicArray(Index initialCapacity) {
    Data = static_cast<T*>(Alloc.allocate(initialCapacity * sizeof(T)));
  }

  ~DynamicArray() {
    Alloc.deallocate(Data);
  }

#pragma region Methods

  [[nodiscard]] T& operator[](Index index) {
    return Data[index];
  }

  [[nodiscard]] const T& operator[](Index index) const {
    return Data[index];
  }

#pragma region Getters

  [[nodiscard]] constexpr Index capacity() const noexcept {
    return Capacity;
  }

  [[nodiscard]] constexpr T* data() const noexcept {
    return Data;
  }

#pragma endregion

#pragma region Memory Management

  void double_capacity() {
    grow(2 * Capacity);
  }

  void grow(Index newCapacity) {
    if constexpr (ReallocatableAllocator<A>)
      Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity));
    else {
      T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
      std::memcpy(newData, Data, Capacity * sizeof(T));
      Alloc.deallocate(Data);
      Data = newData;
    }

    Capacity = newCapacity;
  }

  void reserve(Index newCapacity) {
    if (newCapacity > Capacity) grow(newCapacity);
  }

  void resize(Index newCapacity) {
    if (newCapacity < Capacity) shrink(newCapacity);
    else if (newCapacity > Capacity) grow(newCapacity);
  }

  void shrink(Index newCapacity) {
    if constexpr (ReallocatableAllocator<A>)
      Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity));
    else {
      T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
      std::memcpy(newData, Data, newCapacity * sizeof(T));
      Alloc.deallocate(Data);
      Data = newData;
    }

    Capacity = newCapacity;
  }

#pragma endregion

#pragma endregion

};
