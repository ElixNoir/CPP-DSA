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

    Index Head = 0;
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
        Head = Nodes[Head - 1].Next;
        Nodes.deallocate(Head - 1);
    }

    void discard_next_of(Index index) {
        // IndexNodes[index - 1].Next
    }

#pragma endregion

#pragma region

    [[nodiscard]] Index insert_after() {

    }

#pragma endregion

#pragma region Prepend

    [[nodiscard]] constexpr bool can_prepend() const noexcept {
        return Nodes.can_allocate();
    }

    [[nodiscard]] Index prepend() {
        Index index = Nodes.allocate() + 1;
        Nodes[index - 1].Next = Head;
        Head = index;
        
        return index;
    }

    [[nodiscard]] Index prepend(T& data) {
        Index index = Nodes.allocate() + 1;
        Nodes[index - 1] = { Head, data };
        Head = index;
        
        return index;
    }

#pragma endregion

#pragma region Removal

    [[nodiscard]] constexpr bool can_remove() const noexcept {
        return can_discard();
    }

    [[nodiscard]] T remove_next_of(Index index) {
        T data = Nodes[index - 1].Data;
        discard_next_of(index);
        return data;
    }

    [[nodiscard]] T remove_front() {
        T data = Nodes[Head - 1].Data;
        discard_front();
        return data;
    }

#pragma endregion

#pragma endregion

};
