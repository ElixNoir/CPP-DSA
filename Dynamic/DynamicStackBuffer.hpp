#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>
#include <cstring>
#include <type_traits>

#pragma endregion

/// <summary>
/// A static stack of bytes representable as any trivially copyable type.
/// Does not do any runtime alignment or padding.
/// </summary>
/// <typeparam name="Index">The internal index type, exposed for size tuning.</typeparam>
/// <typeparam name="A">The allocator to use when allocating, deallocating, or reallocating.</typeparam>
template <std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicStackBuffer {
protected:

    [[no_unique_address]] A Alloc;

    std::byte* Data;
    Index Position = 0;
    Index Capacity;

public:

    DynamicStackBuffer(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<std::byte*>(Alloc.allocate(initialCapacity));
    }

    ~DynamicStackBuffer() {
        Alloc.deallocate(Data);
    }

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr std::byte* data() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr Index position() const noexcept {
        return Position;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        Capacity *= 2;
        Data = Alloc.reallocate(Data, Capacity);
    }

    void resize(Index newCapacity) {
        if (Capacity == Position) return;
        Data = Alloc.reallocate(Data, newCapacity);
        if (newCapacity < Capacity) Position = newCapacity;
        Capacity = newCapacity;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) {
            Capacity = newCapacity;
            Data = Alloc.reallocate(Data, Capacity);
        }
    }

    void shrink_to_fit() {
        if (Capacity == Position) return;
        Capacity = Position;
        Data = Alloc.reallocate(Data, Capacity);
    }

#pragma endregion

#pragma region Discard

    template <typename T>
    [[nodiscard]] constexpr bool can_discard(Index count = 1) const noexcept {
        return Position >= count * sizeof(T);
    }

    template <typename T = std::byte>
    void discard(const Index count = 1) {
        Position -= count * sizeof(T);
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T>
    [[nodiscard]] T& peek() {
        return *reinterpret_cast<T*>(Data + Position - sizeof(T));
    }

    template <typename T>
    [[nodiscard]] T& peek(Index position) {
        return *reinterpret_cast<T*>(Data + position - sizeof(T));
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T peek_copy() {
        T value;
        std::memcpy(&value, Data + Position - sizeof(T), sizeof(T));
        return value;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T peek_copy(Index position) {
        T value;
        std::memcpy(&value, Data + position - sizeof(T), sizeof(T));
        return value;
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T pop() {
        Position -= sizeof(T);
        T value;
        std::memcpy(&value, Data + Position, sizeof(T));
        return value;
    }

    template <typename T>
    T& pop_reference() {
        Position -= sizeof(T);
        return *reinterpret_cast<T*>(Data.data() + Position);
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_push(Index count = 1) const noexcept {
        return Position + count * sizeof(T) <= Capacity;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    void push(const T& value) {
        std::memcpy(Data + Position, &value, sizeof(T));
        Position += sizeof(T);
    }

#pragma endregion

#pragma region Setters

    [[nodiscard]] constexpr bool can_position(Index position) const {
        return position < Capacity;
    }

    void position(Index index) {
        Position = index;
    }

#pragma endregion

#pragma endregion

};
