#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#pragma endregion

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
        requires (std::is_trivially_copyable_v<T>)
    T operator[](Index index) {
        T value;
        std::memcpy(&value, Data + index - sizeof(T), sizeof(T));
        return value;
    }

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] uint8_t* data() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        reallocate(2 * Capacity);
    }

    void reallocate(const size_t newCapacity) {
        Data = Alloc.reallocate(Data);
        Capacity = newCapacity;
    }

    void shrink_to_fit() {
        reallocate(Size);
    }

#pragma endregion

#pragma region Discard

    template <typename T>
    [[nodiscard]] constexpr bool can_discard(const size_t count = 1) const noexcept {
        return Size >= count * sizeof(T);
    }

    template <typename T = uint8_t>
    void discard(const Index count) {
        Size -= count * sizeof(T);
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const size_t count = 1) const noexcept {
        return Size + sizeof(T) <= Capacity;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T peek() {
        T value;
        std::memcpy(&value, Data + Size - sizeof(T), sizeof(T));
        return value;
    }

    template <typename T>
    T& peek_reference() {
        return *reinterpret_cast<T*>(Data + Size - sizeof(T));;
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(const size_t count = 1) const noexcept {
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
    [[nodiscard]] constexpr bool can_push(const size_t count = 1) const noexcept {
        return can_peek<T>(count);
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
