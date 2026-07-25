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
    Index Previous;
    T Data;

  };

protected:

  Index Head = 0;
  Index Tail = 0;
  DynamicPool<Node, Index, A> Nodes;

public:

  DynamicLinkedList(initialCapacity) : Nodes(initialCapacity) {}

#pragma region Methods

  [[nodiscard]] T& operator[](Index index) {
    return Nodes[index].Data;
  }

  [[nodiscard]] Index next_of(Index index) {
    return Nodes[index].Next;
  }

  [[nodiscard]] Index previous_of(Index index) {
    return Nodes[index].Previous;
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

  [[nodiscard]] constexpr Index tail() const noexcept {
    return Tail;
  }

#pragma endregion

#pragma region Discard

  [[nodiscard]] constexpr bool can_discard() const noexcept {
    return size() != 0;
  }

  void discard_back() {
    Nodes[Nodes[Tail].Previous].Next = 0;
    Tail = Nodes[Tail].Previous;
    Nodes.deallocate(Tail);
  }

  void discard_front() {
    Nodes[Nodes[Head].Next].Previous = 0;
    Head = Nodes[Head].Next;
    Nodes.deallocate(Head);
  }

#pragma endregion

#pragma region Append

  [[nodiscard]] constexpr bool can_append() const noexcept {
    return Nodes.can_allocate();
  }

  [[nodiscard]] Index append() {
    Index index = Nodes.allocate();
    Nodes[index].Next = 0;
    Nodes[index].Previous = Tail;

    Tail = index;
    if (size() == 0) Head = index;
    
    return index;
  }

  [[nodiscard]] Index append(T data) {
    Index index = Nodes.allocate();
    Nodes[index] = Node { data, Head, 0 };
    
    Tail = index;
    if (size() == 0) Head = index;
    
    return index;
  }

#pragma endregion

#pragma region Prepend

  [[nodiscard]] constexpr bool can_prepend() const noexcept {
    return Nodes.can_allocate();
  }

  [[nodiscard]] Index prepend() {
    Index index = Nodes.allocate();
    Nodes[index].Next = Head;
    Nodes[index].Previous = 0;

    Head = index;
    if (size() == 0) Tail = index;
    
    return index;
  }

  [[nodiscard]] Index prepend(T data) {
    Index index = Nodes.allocate();
    Nodes[index] = Node { data, Head, 0 };
    
    Head = index;
    if (size() == 0) Tail = index;
    
    return index;
  }

#pragma endregion

#pragma region Removal

  [[nodiscard]] constexpr bool can_remove() const noexcept {
    return can_discard();
  }

  [[nodiscard]] T remove(Index index) {
    return remove_reference(index);
  }

  [[nodiscard]] T& remove_reference(Index index) {
    T& data = Nodes[index].Data;

    Index next = Nodes[index].Next;
    Index previous = Nodes[index].Previous;

    Nodes[index].Next = 0;
    Nodes[index].Previous = 0;
    
    Nodes[next].Previous = previous
    Nodes[previous].Next = next;
    
    Nodes.deallocate(index);
    
    return data;
  }

  [[nodiscard]] T remove_back() {
    return remove_back_reference();
  }

  [[nodiscard]] T& remove_back_reference() {
    T& data = Nodes[Tail].Data;
    Index previous = Nodes[Tail].Previous;
    Nodes[previous].Next = 0;
    Nodes.deallocate(Tail);
    Tail = previous;
    return data;
  }

  [[nodiscard]] T remove_front() {
    return remove_front_reference();
  }

  [[nodiscard]] T& remove_front_reference() {
    T& data = Nodes[Head].Data;
    Index next = Nodes[Head].Next;
    Nodes[next].Previous = 0;
    Nodes.deallocate(Head);
    Head = next;
    return data;
  }

#pragma endregion

#pragma endregion

};
