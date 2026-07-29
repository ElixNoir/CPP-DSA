#pragma once

#pragma Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"
#include "StaticBitArray.hpp"

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
  StaticBitArray* Bitmasks;
  Queue* Queues;

  [[nodiscard]] constexpr size_t BitmaskBits = 8 * sizeof(StaticBitArray);

  [[nodiscard]] constexpr size_t BitShift = std::bit_width(8 * sizeof(StaticBitArray) - 1);

  [[nodiscard]] constexpr size_t bit_masks_count() const noexcept {
    return (Capacity + BitmaskBits - 1) >> BitShift;
  }

  [[nodiscard]] constexpr size_t bit_masks_count(size_t maximumPriority) const noexcept {
    return (maximumPriority + BitmaskBits - 1) >> BitShift;
  }

  void internal_resize() {
    const newBitmasksCount = bit_masks_count(newCapacity);
    
    if constexpr (ReallocatableAllocator<A>) {
      Bitmasks = static_cast<StaticBitArray*>(Alloc.reallocate(Bitmasks, newBitmasksCount * sizeof(StaticBitArray)));
    } else {
      StaticBitArray* newBitmasks = static_cast<StaticBitArray*>(Alloc.allocate(newBitmasksCount * sizeof(StaticBitArray)));
      std::memcpy(newBitmasks, Bitmasks, bit_masks_count() * sizeof(StaticBitArray));
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
        ::new (&newQueues[index]) Queue(std::move(Queues[index]));
        if constexpr (!std::is_trivially_destructible_v<Queue>) Queues[index].~Queue();
      }

      Alloc.deallocate(Data);
      Queues = newQueues;
    }
    
    Capacity = newCapacity;
  }

public:

  DynamicBitmappedPriorityQueue(Index initialCapacity) : Capacity(initialCapacity) {
    const size_t bitMasksCount = bit_masks_count();
    Bitmasks = static_cast<StaticBitArray*>(Alloc.allocate(bitMasksCount * sizeof(StaticBitArray)));
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
    return Size;
  }

#pragma endregion

#pragma region Memory Management

  void double_capacity() {
    internal_resize(2 * Capacity);
  }

  void double_capacity_in_subqueue(size_t priority) {
    Queues[priority].double_capacity();
  }

  void reserve(size_t newCapacity) {
    if (newCapacity > Capacity) internal_resize(newCapacity);
  }

  void reserve_in_subqueue(size_t priority, Index newCapacity) {
    Queues[priority].reserve(newCapacity);
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

  [[nodiscard]] constexpr bool can_discard(size_t priority) const noexcept {
    return priority < Capacity && Queues[priority].can_discard();
  }

  [[nodiscard]] constexpr bool can_discard(size_t minimumPriority, size_t maximumPriority) const noexcept {
    return can_discard() && minimumPriority < maximumPriority && maximumPriority < Capacity;
  }

  void discard() {
    discard(0, bit_masks_count());
  }

  void discard(size_t priority) {
    Queue& queue = Queues[priority];
    queue.discard();
    if (queue.is_empty()) StaticBitArray.reset(priority);
    Size--;
  }

  void discard(size_t minimumPriority, size_t maximumPriority) {
    for (size_t index = minimumPriority; index < maximumPriority; index++) {
      StaticBitArray& StaticBitArray = Bitmasks[index];
      size_t priority = StaticBitArray.count_leading_zeros();
      if (priority != 0) {
        priority += 8 * sizeof(StaticBitArray) * index - 1;
        Queue& queue = Queues[priority];
        queue.discard();
        Size--;
        if (queue.is_empty()) StaticBitArray.reset(priority);
        return;
      }
    }
  }

#pragma endregion

#pragma region Dequeue

  [[nodiscard]] constexpr bool can_dequeue() const noexcept {
    return can_discard();
  }

  [[nodiscard]] constexpr bool can_dequeue(size_t priority) const noexcept {
    return can_discard(priority);
  }

  [[nodiscard]] T dequeue() {
    for (size_t index = 0; index < bit_masks_count(); index++) {
      StaticBitArray& StaticBitArray = Bitmasks[index];
      size_t priority = StaticBitArray.count_leading_zeros();
      if (priority != 0) {
        priority += BitmaskBits * index - 1;
        Queue& queue = Queues[priority];
        T value = queue.dequeue();
        if (queue.is_empty()) StaticBitArray.reset(priority);
        Size--;
        return value;
      }
    }
    return T();
  }

  [[nodiscard]] T dequeue(size_t priority) {
    Queue& queue = Queues[priority];
    T value = queue.dequeue();
    if (queue.is_empty()) StaticBitArray.reset(priority);
    Size--;
    return value;
  }

#pragma endregion

#pragma region Enqueue

  [[nodiscard]] constexpr bool can_enqueue(size_t priority) const noexcept {
    return priority < Capacity && Queues[priority].can_enqueue();
  }

  void enqueue(size_t priority, T& value) {
    Queues[priority].enqueue(value);
    StaticBitArray.set(priority);
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
    for (size_t index = 0; index < bit_masks_count(); index++) {
      StaticBitArray& StaticBitArray = Bitmasks[index];
      size_t priority = StaticBitArray.count_leading_zeros();
      if (priority != 0) {
        priority += BitmaskBits * index - 1;
        return Queues[priority].peek();
      }
    }
  }

  T& peek(size_t priority) {
    return Queues[priority].peek();
  }

#pragma endregion

};
