#pragma once

#pragma region Dependencies

#include "DSAConcepts.hpp"

#include <cstddef>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicBinaryTree {
public:

  struct Node {

    Index Left;
    Index Right;
    T Key;

  };

protected:

  [[no_unique_address]] A Alloc;

  T* Data;
  Index Root = 0;
  Index FreeRoot = 0;
  Index Size = 0;
  Index Capacity;

public:

  DynamicBinaryTree(Index initialCapacity) : Capacity(initialCapacity) {
    Data = static_cast<T*>(Alloc.allocate(initialCapacity * sizeof(Node)));
  }

  ~DynamicBinaryTree() {
    Alloc.deallocate(Data);
  }

#pragma region Getters

  [[nodiscard]] T& operator[](Index index) const {
    return Data[index].Key;
  }

  [[nodiscard]] const T& operator[](const Index index) const {
    return Data[index].Key;
  }

  [[nodiscard]] constexpr Index left_of(Index index) const {
    return Data[index].Left;
  }

  [[nodiscard]] constexpr Index right_of(Index index) const {
    return Data[index].Right;
  }

#pragma endregion

#pragma region Memory Management

  [[nodiscard]] constexpr bool can_allocate(Index count = 1) const noexcept {
    return Size + count <= Capacity;
  }

  [[nodiscard]] const Index allocate() {
    Index index = FreeRoot;
    if (index != 0) address = Data[index].Right;
    else index = ++Size;
    return index;
  }

  [[nodiscard]] constexpr bool can_deallocate(Index count = 1) const noexcept {
    return Size >= count;
  }

  void deallocate(Index index) {
    Data[index].Right = FreeRoot;
    FreeRoot = index;
    Size--;
  }

  void double_capacity() {
    grow(2 * Capacity);
  }

  void grow(Index newCapacity) {
    if constexpr (ReallocatableAllocator<A>) {
      Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity * sizeof(T)));
    } else {
      T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
      if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(newData, Data, Capacity * sizeof(T));
      else {
        // traversal to call destructors
      }
      
      Alloc.deallocate(Data);
      Data = newData;
    }

    Capacity = newCapacity;
  }

  void reserve(Index newCapacity) {
    if (newCapacity > Capacity) grow(newCapacity);
  }

#pragma endregion

#pragma region Modification

  [[nodiscard]] constexpr bool can_upsert() const noexcept {
    return can_allocate();
  }

  [[nodiscard]] Index upsert(const T& key) {
    Index currentIndex = Root;
    if (currentIndex == 0) {
      Root = allocate();
      Data[Root] = { 0, 0, key };
      return Root;
    }

    while (true) {
      T& currentKey = Data[currentIndex].Key;
      if (current.Key == key) {
        Data[currentIndex] = { 0, 0, key };
        return 0;
      } else if (currentKey > key) {
        Index leftIndex = Data[currentIndex].Left;
        if (leftIndex != 0) currentIndex = leftIndex;
        else {
          Index index = allocate();
          Data[index] = { 0, 0, key };
          Data[currentIndex].Left = index;
          return index;
        }
      } else {
        Index rightIndex = Data[currentIndex].Right;
        if (rightIndex != 0) currentIndex = rightIndex;
        else {
          Index index = allocate();
          Data[index] = { 0, 0, key };
          Data[currentIndex].Right = index;
          return index;
        }
      }
    }
  }

  [[nodiscard]] constexpr bool can_remove() const noexcept {
    return Size != 0;
  }

  [[nodiscard]] Index remove(const T& key) {
    Index currentIndex = find(key);
    if (currentIndex == 0) return 0;
    
    Index parentIndex = 0;
    Index leftIndex = Data[currentIndex].Left;
    Index rightIndex = Data[currentIndex].Right;
    if (Data[currentIndex].Left == 0 || Data[currentIndex].Right == 0) {
      Index replacementIndex = leftIndex != 0 ? leftIndex : rightIndex;
      if (parentIndex == 0) Root = replacementIndex;
      else if (Data[parentIndex].Left == currentIndex) Data[parentIndex].Left = replacementIndex;
      else Data[parentIndex].Right = replacementIndex;
    }

    parentIndex = currentIndex;
    leftIndex = Data[rightIndex].Left;
    while (leftIndex != 0) {
      parentIndex = rightIndex;
      rightIndex = leftIndex;
      leftIndex = Data[rightIndex].Left;
    }

    Data[currentIndex] = Data[rightIndex];

    Index replacementIndex = Data[rightIndex].Right;
    if (parentIndex == currentIndex) Data[parentIndex].Right = replacementIndex;
    else Data[parentIndex].Left = replacementIndex;

    deallocate(rightIndex);
    
    return currentIndex;
  }

#pragma endregion

#pragma region Ordering

  [[nodiscard]] constexpr bool can_copy_order() const noexcept {
    return Root != 0;
  }

  void copy_level_order(Index* data) const {
    Index current = Root;
    Index head = 0;
    Index top = 0;
    
    while (head <= top) {
      data[top++] = current;

      Index left = Data[current].Left;
      if (left != 0) data[top++] = left;

      Index right = Data[current].Right;
      if (right != 0) data[top++] = right;

      current = data[++head];
    }
  }

  void copy_post_order(Index* data) const {
    Index current = Root;
    Index top = 0;
    while (current != 0) {
      data[top++] = current;
      
      Index left = Data[current].Left;
      if (left != 0) data[top++] = left;

      Index right = Data[current].Right;
      if (right != 0) data[top++] = right;
  
      current = data[--top];
    }
  }

  void copy_pre_order(Index* data) const {
    Index current = Root;
    Index top = 0;
    while (current != 0) {
      data[top++] = current;

      Index right = Data[current].Right;
      if (right != 0) data[top++] = right;
      
      Index left = Data[current].Left;
      if (left != 0) data[top++] = left;

      current = data[--top];
    }
  }

#pragma endregion

#pragma region Search

  [[nodiscard]] Index ceiling(const T& key) {
    Index currentIndex = Root;
    Index candidate = currentIndex;
    while (currentIndex != 0) {
      T& currentKey = Data[currentIndex].Key;
      if (currentKey == key) return currentIndex;
      else if (currentKey > key) {
        candidate = currentIndex;
        currentIndex = Data[currentIndex].Left;
      } else currentIndex = Data[currentIndex].Right;
    }
    return candidate;
  }

  [[nodiscard]] Index find(const T& key) const {
    Index currentIndex = Root;
    while (currentIndex != 0) {
      T& currentKey = Data[currentIndex].Key;
      if (currentKey == key) return currentIndex;
      currentIndex = (currentKey > key) ? Data[current].Left : Data[current].Right;
    }
    return 0;
  }

  [[nodiscard]] Index floor(const T& key) const {
    Index currentIndex = Root;
    Index candidate = currentIndex;
    while (currentIndex != 0) {
      T& currentKey = Data[currentIndex].Key;
      if (currentKey == key) return currentIndex;
      else if (currentKey < key) {
        candidate = currentIndex;
        currentIndex = Data[currentIndex].Right;
      } else currentIndex = Data[currentIndex].Left;
    }
    return candidate;
  }

#pragma endregion

};
