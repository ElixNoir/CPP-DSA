#pragma once

#pragma region Dependencies

#include "DynamicPool.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
    requires std::is_trivially_copyable_v<T>
class DynamicLinkedList {
public:

  struct Node {

    Index Next;
    T Data;

  };

protected:

  DynamicPool<Node, Index, A> Nodes;
  Index Head = 0;

public:

  DynamicLinkedList(initialCapacity) : Nodes(initialCapacity) {}

#pragma region Methods

  [[nodiscard]] T& operator[](Index index) {
    return Nodes[index].Data;
  }

  [[nodiscard]] Index next_of(Index index) {
    return Nodes[index].Next;
  }

#pragma region Getters

  [[nodiscard]] constexpr Node* data() const noexcept {
    return Nodes.data();
  }

  [[nodiscard]] constexpr Index head() const noexcept {
    return Head;
  }

  [[nodiscard]] constexpr Index size() const noexcept {
    return Nodes.size();
  }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        Nodes.double_capacity();
    }

    void reserve(Index newCapacity) {
        Nodes.reserve(newCapacity);
    }

#pragma endregion

#pragma region Discard

  [[nodiscard]] constexpr bool can_discard() const noexcept {
    return size() != 0;
  }

  void discard_front() {
    Index next = Nodes[Head].Next;
    Nodes.deallocate(Head);
    Head = next;
  }

#pragma endregion

#pragma region Prepend

  [[nodiscard]] constexpr bool can_prepend() const noexcept {
    return Nodes.can_allocate();
  }

  [[nodiscard]] Index prepend() {
    Index index = Nodes.allocate();
    Nodes[index].Next = Head;
    Head = index;
    
    return index;
  }

  [[nodiscard]] Index prepend(T data) {
    Index index = Nodes.allocate();
    Nodes[index] = Node { data, Head };
    Head = index;
    
    return index;
  }

#pragma endregion

#pragma region Removal

  [[nodiscard]] constexpr bool can_remove() const noexcept {
    return can_discard();
  }

  [[nodiscard]] T remove_next_of(Index index) {
    return remove_next_of_reference(index);
  }

  [[nodiscard]] T& remove_next_of_reference(Index index) {
    Index next = Nodes[index].Next;
    T& data = Nodes[next].Data;
    Nodes[index].Next = Nodes[next].Next;
    Nodes.deallocate(next);
    return data;
  }

  [[nodiscard]] T remove_front() {
    return remove_front_reference();
  }

  [[nodiscard]] T& remove_front_reference() {
    T& data = Nodes[Head].Data;
    Index next = Nodes[Head].Next;
    Nodes.deallocate(Head);
    Head = next;
    return data;
  }

#pragma endregion

#pragma endregion

};
