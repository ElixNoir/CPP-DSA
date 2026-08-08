#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#pragma endregion

template <Comparable T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicAVLTree {
public:

  struct Node {

    Index Left;
    Index Right;
    Index Height;
    T Key;

  };

protected:

  [[no_unique_address]] A Alloc;

  T* Data;
  Index Capacity;
  Index Size = 0;
  Index Root = 0;
  Index FreeRoot = 0;

public:

#pragma region Memory Management

  [[nodiscard]] constexpr bool can_allocate(Index count = 1) const noexcept {
    return Size + count <= Capacity;
  }

  [[nodiscard]] const Index allocate() {
    Index index = FreeRoot;
    if (index != 0) address = Data[index - 1].Right;
    else index = ++Size;
    return index;
  }

  [[nodiscard]] constexpr bool can_deallocate(Index count = 1) const noexcept {
    return Size >= count;
  }

  void deallocate(Index index) {
    Data[index - 1].Right = FreeRoot;
    FreeRoot = index;
    Size--;
  }

  void double_capacity() {
    grow(2 * Capacity);
  }

  void grow(Index newCapacity) {
    if constexpr (ReallocatableAllocator<A>) {
      Data = static_cast<Node*>(Alloc.reallocate(Data, newCapacity * sizeof(Node)));
    } else {
      Node* newData = static_cast<Node*>(Alloc.allocate(newCapacity * sizeof(Node)));
      //if constexpr (std::is_trivially_copyable_v<Node>)
      std::memcpy(newData, Data, Capacity * sizeof(Node));
      //else {
        // traversal to call destructors
      //}
      
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

  [[nodiscard]] Index rotate_left(Index currentIndex) {
    Index leftIndex = Data[currentIndex].Left;
    Index rightIndex = Data[currentIndex].Right;
    
    Index rightLeftIndex = Data[rightIndex].Left;
    Index rightRightIndex = Data[rightIndex].Right;
    
    Data[rightIndex].Left = currentIndex;
    Data[currentIndex].Right = rightLeftIndex;

    Index leftHeight = (leftIndex != 0 ? Data[leftIndex].Height : 0);
    Index rightHeight = (rightIndex != 0 ? Data[rightIndex].Height : 0);
    Data[currentIndex].Height = (leftHeight >= rightHeight ? leftHeight : rightHeight) + 1;
    
    leftHeight = (rightLeftIndex != 0 ? Data[rightLeftIndex].Height : 0);
    rightHeight = (rightRightIndex != 0 ? Data[rightRightIndex].Height : 0);
    Data[rightIndex].Height = (leftHeight >= rightHeight ? leftHeight : rightHeight) + 1;

    return rightIndex;
  }

  [[nodiscard]] Index rotate_right(Index currentIndex) {
    Index leftIndex = Data[currentIndex].Left;
    Index rightIndex = Data[currentIndex].Right;
    
    Index leftLeftIndex = Data[leftIndex].Left;
    Index leftRightIndex = Data[leftIndex].Right;
    
    Data[leftIndex].Right = currentIndex;
    Data[currentIndex].Left = leftRightIndex;

    Index leftHeight = (leftIndex != 0 ? Data[leftIndex].Height : 0);
    Index rightHeight = (rightIndex != 0 ? Data[rightIndex].Height : 0);
    Data[currentIndex].Height = (leftHeight >= rightHeight ? leftHeight : rightHeight) + 1;
    
    leftHeight = (leftLeftIndex != 0 ? Data[leftLeftIndex].Height : 0);
    rightHeight = (leftRightIndex != 0 ? Data[leftRightIndex].Height : 0);
    Data[rightIndex].Height = (leftHeight >= rightHeight ? leftHeight : rightHeight) + 1;

    return leftIndex;
  }

  [[nodiscard]] Index rebalance(Index currentIndex) {
    Index leftIndex = Data[currentIndex].Left;
    Index rightIndex = Data[currentIndex].Right;
    
    Index leftHeight = (leftIndex != 0 ? Data[leftIndex].Height : 0);
    Index rightHeight = (rightIndex != 0 ? Data[rightIndex].Height : 0);
  
    if (leftHeight > rightHeight + 1) {
      Data[currentIndex].Height = leftHeight + 1;
      Index leftLeftIndex = Data[leftIndex].Left;
      Index leftRightIndex = Data[leftIndex].Right;
      if ((leftLeftIndex != 0 ? Data[leftLeftIndex].Height : 0)
        < (leftRightIndex != 0 ? Data[leftRightIndex].Height : 0))
        Data[currentIndex].Left = rotate_left(leftIndex);
      return rotateRight(currentIndex);
    }

    Data[currentIndex].Height = rightHeight + 1;
    
    if (leftHeight + 1 < rightHeight) {
      Index rightLeftIndex = Data[rightIndex].Left;
      Index rightRightIndex = Data[rightIndex].Right;
      if ((rightLeftIndex != 0 ? Data[rightLeftIndex].Height : 0)
        < (rightRightIndex != 0 ? Data[rightRightIndex].Height : 0))
        Data[currentIndex].Right = rotate_right(rightIndex);
      return rotateLeft(currentIndex);
    }
  
    return currentIndex;
  }

#pragma endregion

#pragma region Search

  [[nodiscard]] Index find(const T& key, Index currentIndex) {
    while (currentIndex != 0) {
      const T& currentKey = Data[currentIndex];
      if (key < currentKey) currentIndex = Data[currentIndex].Left;
      else if (key > currentKey) currentIndex = Data[currentIndex].Right;
      else return address
    }
    
    return 0;
  }

#pragma endregion

};
