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

    void discard(Index index) {
        index--;
        
        Index next = Nodes[index].Next;
        Index previous = Nodes[index].Previous;

        if (next != 0) Nodes[next - 1].Previous = previous;
        else Tail = previous;
        
        if (previous != 0) Nodes[previous - 1].Next = next;
        else Head = next;
    
        Nodes.deallocate(index);
    }

    void discard_back() {
        Index previous = Nodes[back - 1].Previous;
    
        if (previous != 0) Nodes[back - 1].Next = 0;
        else Head = 0;

        Index back = Tail;
        Tail = previous;
        Nodes.deallocate(back - 1);
    }

    void discard_front() {
        Index next = Nodes[front - 1].Next;
    
        if (next != 0) Nodes[next - 1].Previous = 0;
        else Tail = 0;

        Index front = Head;
        Head = next;
        Nodes.deallocate(front - 1);
    }

#pragma endregion

#pragma region Append

    [[nodiscard]] constexpr bool can_append() const noexcept {
        return Nodes.can_allocate();
    }

    [[nodiscard]] Index append() {
        Index index = Nodes.allocate() + 1;

        Nodes[index - 1] = { 0, Tail };
    
        if (Tail != 0) Nodes[Tail - 1].Next = index;
        else Head = index;
        Tail = index;
        
        return index;
    }

    [[nodiscard]] Index append(T& data) {
        Index index = Nodes.allocate() + 1;
            
        Nodes[index - 1] = { 0, Tail, data };
    
        if (Tail != 0) Nodes[Tail - 1].Next = index;
        else Head = index;
        Tail = index;
        
        return index;
    }

#pragma endregion

#pragma region Insert

    [[nodiscard]] constexpr bool can_insert() const noexcept {
        return Nodes.can_allocate();
    }

    [[nodiscard]] Index insert_after(Index current) {
        Index index = Nodes.allocate() + 1;
        Index next = Nodes[current - 1].Next;
    
        Nodes[index - 1] = { next, current };
        Nodes[current - 1].Next = index;
    
        if (next != 0) Nodes[next - 1].Previous = index;
        else Tail = index;
    
        return index;
    }

    [[nodiscard]] Index insert_after(Index current, T& data) {
        Index index = Nodes.allocate() + 1;
        Index next = Nodes[current - 1].Next;
    
        Nodes[index - 1] = { next, current, data };
        Nodes[current - 1].Next = index;
    
        if (next != 0) Nodes[next - 1].Previous = index;
        else Tail = index;
    
        return index;
    }

    [[nodiscard]] Index insert_before(Index current) {
        Index index = Nodes.allocate() + 1;
        Index previous = Nodes[current - 1].Previous;
    
        Nodes[index - 1] = { current, previous };
        Nodes[current - 1].Previous = index;
    
        if (previous != 0) Nodes[previous - 1].Next = index;
        else Head = index;
    
        return index;
    }

    [[nodiscard]] Index insert_before(Index current, T& data) {
        Index index = Nodes.allocate() + 1;
        Index previous = Nodes[current - 1].Previous;
    
        Nodes[index - 1] = { current, previous, data };
        Nodes[current - 1].Previous = index;
    
        if (previous != 0) Nodes[previous - 1].Next = index;
        else Head = index;
    
        return index;
    }

#pragma endregion

#pragma region Prepend

    [[nodiscard]] constexpr bool can_prepend() const noexcept {
        return Nodes.can_allocate();
    }

    [[nodiscard]] Index prepend() {
        Index index = Nodes.allocate() + 1;
            
        Nodes[index - 1] = { Head, 0 };
    
        if (Head != 0) Nodes[Head - 1].Previous = index;
        else Tail = index;
        Head = index;
        
        return index;
    }

    [[nodiscard]] Index prepend(T& data) {
        Index index = Nodes.allocate() + 1;
            
        Nodes[index - 1] = { Head, 0, data };
        
        if (Head != 0) Nodes[Head - 1].Previous = index;
        else Tail = index;
        Head = index;
        
        return index;
    }

#pragma endregion

#pragma region Removal

    [[nodiscard]] constexpr bool can_remove() const noexcept {
        return can_discard();
    }

    [[nodiscard]] T remove(Index index) {
        T data = Nodes[index - 1].Data;
        discard(index);
        return data;
    }

    [[nodiscard]] T remove_back() {
        T data = Nodes[Tail - 1].Data;
        discard_back();
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
