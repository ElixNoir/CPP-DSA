#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>
#include <cstdint>
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

    uint8_t* Data;
    Index Size = 0;
    Index Capacity;

public:

    DynamicStackBuffer(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<uint8_t*>(Alloc.allocate(initialCapacity));
    }

    ~DynamicStackBuffer() {
        Alloc.deallocate(Data);
    }

#pragma region Methods

    template <typename T>
    [[nodiscard]] T& operator[](Index index) {
        return *reinterpret_cast<T*>(Data + index);
    }

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr uint8_t* data() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        Capacity *= 2;
        Data = Alloc.reallocate(Data, Capacity);
    }

    void resize(Index newCapacity) {
        if (Capacity == Size) return;
        Data = Alloc.reallocate(Data, newCapacity);
        if (newCapacity < Capacity) Size = newCapacity;
        Capacity = newCapacity;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) {
            Capacity = newCapacity;
            Data = Alloc.reallocate(Data, Capacity);
        }
    }

    void shrink_to_fit() {
        if (Capacity == Size) return;
        Capacity = Size;
        Data = Alloc.reallocate(Data, Capacity);
    }

#pragma endregion

#pragma region Discard

    template <typename T>
    [[nodiscard]] constexpr bool can_discard(const Index count = 1) const noexcept {
        return Size >= count * sizeof(T);
    }

    template <typename T = uint8_t>
    void discard(const Index count = 1) {
        Size -= count * sizeof(T);
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T>
    [[nodiscard]] T& peek() {
        return *reinterpret_cast<T*>(Data + Size - sizeof(T));
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T peek_copy() {
        T value;
        std::memcpy(&value, Data + Size - sizeof(T), sizeof(T));
        return value;
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T pop() {
        Size -= sizeof(T);
        T value;
        std::memcpy(&value, Data + Size, sizeof(T));
        return value;
    }

    template <typename T>
    T& pop_reference() {
        Size -= sizeof(T);
        return *reinterpret_cast<T*>(Data.data() + Size);
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_push(const Index count = 1) const noexcept {
        return Size + count * sizeof(T) <= Capacity;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    void push(const T& value) {
        std::memcpy(Data + Size, &value, sizeof(T));
        Size += sizeof(T);
    }

#pragma endregion

#pragma endregion

};
