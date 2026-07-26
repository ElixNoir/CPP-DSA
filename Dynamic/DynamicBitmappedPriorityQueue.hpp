#pragma once

#pragma Dependencies

#include "BitArray.hpp"
#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <type_traits>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicBitmappedPriorityQueue {
protected:

  using Queue = DynamicWrappedQueue<T, Index, A>;

  [[no_unique_address]] A Alloc;
  size_t Capacity;
  size_t Size = 0;
  BitArray* Bitmasks;
  Queue* Queues;

  [[nodiscard]] constexpr size_t bit_masks_count() const noexcept {
    return (Capacity + 63) >> 6;
  }

  [[nodiscard]] constexpr size_t bit_masks_count(size_t newCapacity) const noexcept {
    return (newCapacity + 63) >> 6;
  }

  void internal_resize() {
    const newBitMasksCount = bit_masks_count(newCapacity);
    
    if constexpr (ReallocatableAllocator<A>) {
      Bitmasks = static_cast<BitArray*>(Alloc.reallocate(Bitmasks, newBitMasksCount * sizeof(BitArray)));
    } else {
      BitArray* newBitmasks = static_cast<BitArray*>(Alloc.allocate(newBitMasksCount * sizeof(BitArray)));
      std::memcpy(newBitmasks, Bitmasks, bit_masks_count() * sizeof(BitArray));
      Bitmasks = newBitmasks;
    }
    
    if constexpr (std::is_trivially_copyable_v<Queue>) {
      if constexpr (ReallocatableAllocator<A>)
        Queues = static_cast<Queue*>(Alloc.reallocate(Queues, newCapacity * sizeof(Queue)));
      else {
        Queue* newQueues = static_cast<Queue*>(Alloc.allocate(newCapacity * sizeof(Queue)));
        std::memcpy(newQueues, Queues, Capacity * sizeof(Queue));
        Queues = newQueues;
      }
    } else {
      Queue* newQueues = static_cast<Queue*>(Alloc.allocate(newCapacity * sizeof(Queue)));

      for (size_t index = 0; index < Capacity; index++) {
        ::new (&newData[index]) Queue(std::move(Data[index]));
        if constexpr (!std::is_trivially_destructible_v<Queue>) Data[index].~Queue();
      }

      Alloc.deallocate(Data);
      Queues = newQueues;
    }
    
    Capacity = newCapacity;
  }

public:

  DynamicBitmappedPriorityQueue(Index initialCapacity) : Capacity(initialCapacity) {
    const size_t bitMasksCount = bit_masks_count();
    Bitmasks = static_cast<BitArray*>(Alloc.allocate(bitMasksCount * sizeof(BitArray)));
    Queues = static_cast<Queue*>(Alloc.allocate(initialCapacity * sizeof(Queue)));
    std::fill(Bitmasks, Bitmasks + bitMasksCount, 0);
  }

  ~DynamicBitmappedPriorityQueue() {
    Alloc.deallocate(Bitmasks);
    Alloc.deallocate(Queues);
  }

#pragma region Getters

  [[nodiscard]] constexpr size_t capacity() const noexcept {
    return Capacity;
  }

  [[nodiscard]] constexpr size_t size() const noexcept {
    return Capacity;
  }

#pragma endregion

#pragma region Memory Management

  void double_capacity() {
    internal_resize(2 * Capacity);
  }

  void reserve(size_t newCapacity) {
    if (newCapacity > Capacity) internal_resize(newCapacity);
  }

  /*void resize(size_t newCapacity) {
    if (newCapacity < Size) {
      // do something
    }
    
    internal_resize(newCapacity);
  }*/

  /*void shrink_to_fit() {
    // possibly enable this in O(1) if a "top priority" or "top bitmask" is tracked
  }*/

#pragma endregion

#pragma region Discard

  [[nodiscard]] constexpr bool can_discard() const noexcept {
    return Size != 0;
  }

  void discard() {
    const size_t bitMasksCount = bit_masks_count();
    for (size_t index = 0; index < bitMasksCount; index++) {
      BitArray& bitArray = Bitmasks[index];
      size_t priority = bitArray.count_leading_zeros();
      if (priority != 0) {
        priority += 64 * index - 1;
        Queue& queue = Queues[priority];
        queue.discard();
        Size--;
        if (queue.is_empty()) bitArray.reset(priority);
        return;
      }
    }
  }

  void discard(size_t priority) {
    Queue& queue = Queues[priority];
    queue.discard();
    if (queue.is_empty()) bitArray.reset(priority);
    Size--;
  }

#pragma endregion

#pragma region Dequeue

  [[nodiscard]] constexpr bool can_dequeue() const noexcept {
    return can_discard();
  }

  [[nodiscard]] T dequeue() {
    const size_t bitMasksCount = bit_masks_count();
    for (size_t index = 0; index < bitMasksCount; index++) {
      BitArray& bitArray = Bitmasks[index];
      size_t priority = bitArray.count_leading_zeros();
      if (priority != 0) {
        priority += 64 * index - 1;
        Queue& queue = Queues[priority];
        T value = queue.dequeue();
        if (queue.is_empty()) bitArray.reset(priority);
        Size--;
        return value;
      }
    }
    return T();
  }

  [[nodiscard]] T dequeue(size_t priority) {
    Queue& queue = Queues[priority];
    T value = queue.dequeue();
    if (queue.is_empty()) bitArray.reset(priority);
    Size--;
    return value;
  }

#pragma endregion

#pragma region Enqueue

  [[nodiscard]] constexpr bool can_enqueue(size_t priority) const noexcept {
    return priority < Capacity;
  }

  void enqueue(size_t priority, T& value) {
    Queues[priority].enqueue(value);
    bitArray.set(priority);
    Size++;
  }

#pragma endregion

#pragma region Peek

  [[nodiscard]] constexpr bool can_peek() const noexcept {
    return can_discard();
  }

  [[nodiscard]] constexpr bool can_peek(size_t priority) const noexcept {
    return priority < Capacity;
  }

  T& peek() {
    const size_t bitMasksCount = bit_masks_count();
    for (size_t index = 0; index < bitMasksCount; index++) {
      BitArray& bitArray = Bitmasks[index];
      size_t priority = bitArray.count_leading_zeros();
      if (priority != 0) {
        priority += 64 * index - 1;
        return Queues[priority].peek();
      }
    }
  }

  T& peek(size_t priority) {
    return Queues[priority].peek();
  }

#pragma endregion

};
