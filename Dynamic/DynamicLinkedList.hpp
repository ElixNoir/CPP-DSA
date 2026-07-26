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

    [[nodiscard]] constexpr bool can_discard_next_of(Index current) const noexcept {
        return can_discard() && current != 0;
    }

    void discard_front() {
        Head = Nodes[Head - 1].Next;
        Nodes.deallocate(Head - 1);
    }

    void discard_next_of(Index current) {
        Index next = Nodes[current - 1].Next;
        if (next == 0) return;
        Nodes[current - 1].Next = Nodes[next - 1].Next;
    }

#pragma endregion

#pragma region

    [[nodiscard]] constexpr bool can_insert() const noexcept {
        return Nodes.can_allocate();
    }

    [[nodiscard]] constexpr bool can_insert_after(Index current) const noexcept {
        return can_insert() && current != 0;
    }

    [[nodiscard]] Index insert_after(Index current) {
        Index index = Nodes.allocate() + 1;
        Nodes[index - 1].Next = Nodes[current - 1].Next;
        Nodes[current - 1].Next = index;
        return index;
    }

    [[nodiscard]] Index insert_after(Index current, T& data) {
        Index index = Nodes.allocate() + 1;
        Nodes[index - 1] = { Nodes[current - 1].Next, data };
        Nodes[current - 1].Next = index;
        return index;
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

    [[nodiscard]] constexpr bool can_remove_next_of() const noexcept {
        return can_remove() && current != 0;
    }

    [[nodiscard]] T remove_next_of(Index current) {
        T data = Nodes[current - 1].Data;
        discard_next_of(current);
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
