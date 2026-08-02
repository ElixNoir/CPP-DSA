#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index, Allocator A = DefaultAllocator>
class DynamicGapBuffer {
protected:

  [[no_unique_address]] A Alloc;

  T* Data;
  Index End;
  Index Start = 0;
  Index Capacity;

public:

  DynamicGapBuffer(Index initialCapacity) : Capacity(initialCapacity), End(initialCapacity) {
    Data = static_cast<T*>(Alloc.allocate(initialCapcity * sizeof(T)));
  }

  ~DynamicGapBuffer() {
    Alloc.deallocate(Data);
  }

#pragma region Getters

  [[nodiscard]] constexpr T* data() const noexcept {
    return Data;
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return size() == 0;
  }

  [[nodiscard]] constexpr Index end() const noexcept {
    return End;
  }

  [[nodiscard]] constexpr Index gap_size() const noexcept {
    return End - Start;
  }

  [[nodiscard]] constexpr Index size() const noexcept {
    return Capacity - gap_size();
  }

  [[nodiscard]] constexpr Index start() const noexcept {
    return Start;
  }

#pragma endregion

#pragma region Memory Management

  void double_capacity() {
    grow(2 * End);
  }

  void grow(Index newCapacity)
  {
    if (newCapacity <= Capacity) return;

    T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));

    const Index leftSize  = Start;
    const Index rightSize = Capacity - End;

    if constexpr (std::is_trivially_copyable_v<T>) {
      std::memcpy(newData, Data, leftSize * sizeof(T));
      std::memcpy(newData + newCapacity - rightSize, Data + End, rightSize * sizeof(T));
    } else {
      for (Index index = 0; index < leftSize; index++)
        new (newData + index)
          T(std::move(Data[index]));

      for (Index index = 0; index < rightSize; index++)
        new (newData + newCapacity - rightSize + index)
          T(std::move(Data[End + index]));

      for (Index index = 0; index < leftSize; index++)
        Data[index].~T();

      for (Index index = End; index < End + rightSize; index++)
        Data[index].~T();
    }

    Alloc.deallocate(Data);

    End = newCapacity - rightSize;
    Capacity = newCapacity;
    Data = newData;
  }

  void reserve(Index newCapacity) {
    if (newCapacity > Capacity) grow(newCapacity);
  }

#pragma endregion

  [[nodsicard]] constexpr bool can_insert(Index count = 1) const noexcept {
    return gap_size() >= count;
  }

  void position(Index position) {
    if (position < Start) {
      const Index shift = Start - position;
      if constexpr (std::is_trivially_copyable_v<T>())
        for (Index index = 0; index < shift; index++)
          Data[End - index] = std::move(Data[Start - index]);
      else std::memcpy(Data + position, Data + End - shift, shift * sizeof(T));
      
      End -= Shift;
      Start -= shift;
    } else if (position > Start) {
      const Index shift = position - Start;
      if constexpr (std::is_trivially_copyable_v<T>())
        for (Index index = 0; index < shift; index++)
          Data[Start + index] = std::move(Data[End + index]);
      else std::memcpy(Data + Start, Data + End, shift * sizeof(T));
        
      End += shift;
      Start += shift;
    }
  }

  void insert(const T& value) {
    if constexpr (std::is_trivially_copyable_v<T>) Data[Start] = value;
    else new (Data + Start) T(std::move(value));
    
    Start++;
  }

  void insert(const T* values, Index count) {
    if constexpr (std::is_trivially_copyable_v<T>)
      std::memcpy(Data + Start, values, count * sizeof(T));
    else
      for (Index index = 0; index < count; index++)
        new (Data + Start + index) T(std::move(value));
    
    Start += count;
  }

  void remove(Index count = 1) {
    if constexpr (!std::is_trivially_destructible_v<T>)
      for (Index i = 0; i < count; ++i)
        Data[GapEnd + i].~T();
    
    End += count;
  }

};
