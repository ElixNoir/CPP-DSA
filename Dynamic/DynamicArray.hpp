#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index, Allocator A>
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
    if constexpr (std::is_trivially_copyable_v<T>) {
      if constexpr (ReallocatableAllocator<A>)
        Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity));
      else {
        T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
        std::memcpy(newData, Data, Capacity * sizeof(T));
        Alloc.deallocate(Data);
        Data = newData;
      }
    } else {
      /*

      // may need a kind of set for this, since some may not have been constructed in the first place
      T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));

      for (Index index = 0; index < Capacity; index++) {
        ::new (&newData[index]) T(std::move(Data[index]));
        if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
      }

      Alloc.deallocate(Data);
      Data = newData;
      */
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
    if constexpr (std::is_trivially_copyable_v<T>) {
      if constexpr (ReallocatableAllocator<A>)
        Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity));
      else {
        T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
        std::memcpy(newData, Data, newCapacity * sizeof(T));
        Alloc.deallocate(Data);
        Data = newData;
      }
    } else {
      /*

      // may need a kind of set for this, since some may not have been constructed in the first place
      
      T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));

      for (Index index = 0; index < newCapacity; index++) {
        ::new (&newData[index]) T(std::move(Data[index]));
        if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
      }

      Alloc.deallocate(Data);
      Data = newData;
      */
    }

    Capacity = newCapacity;
  }

#pragma endregion

#pragma endregion

};
