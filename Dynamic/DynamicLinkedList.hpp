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

#pragma endregion

  void discard_front() {
    Nodes.deallocate(Head);
    Head = Nodes[Head].Next;
  }

#pragma region Prepend

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

  [[nodiscard]] T remove_next_of(Index index) {
    Index next = Nodes[index].Next;
    Nodes[index].Next = Nodes[next].Next;
    Nodes.deallocate(next);
    return Nodes[next].Data;
  }

  [[nodiscard]] T& remove_next_of_reference(Index index) {
    Index next = Nodes[index].Next;
    Nodes[index].Next = Nodes[next].Next;
    Nodes.deallocate(next);
    return Nodes[next].Data;
  }

  [[nodiscard]] T remove_front() {
    Nodes.deallocate(Head);
    Head = Nodes[Head].Next;
    return Nodes[Head].Data;
  }

  [[nodiscard]] T& remove_front_reference() {
    Nodes.deallocate(Head);
    Head = Nodes[Head].Next;
    return Nodes[Head].Data;
  }

#pragma endregion

#pragma endregion

};
