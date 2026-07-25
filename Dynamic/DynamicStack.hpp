#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicStack {
protected:

    [[no_unique_address]] A Alloc;

    T* Data;
    Index Size = 0;
    Index Capacity;

    void internal_resize(Index newCapacity) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            if constexpr (ReallocatableAllocator<A>)
                Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity * sizeof(T)));
            else {
                T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
                std::memcpy(newData, Data, newCapacity * sizeof(T));
                Data = newData;
            }
        } else {
            T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));

            for (Index index = 0; index < Size; index++) {
                ::new (&newData[index]) T(std::move(Data[index]));
                if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
            }

            Alloc.deallocate(Data);
            Data = newData;
        }
        
        Capacity = newCapacity;
    }

public:

    DynamicStack(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<T*>(Alloc.allocate(Capacity * sizeof(T)));
    }

    ~DynamicStack() {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index index = 0; index < Size; index++)
                Data[index].~T();
        Alloc.deallocate(Data);
    }

#pragma region Methods

    [[nodiscard]] T& operator[](const Index index) {
        return Data[index];
    }

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr T* data() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        internal_resize(2 * Capacity);
    }

    void reserve(Index newCapacity)
    {
        if (newCapacity > Capacity) internal_resize(newCapacity);
    }

    void resize(Index newCapacity) {
        if (newCapacity == Capacity) return;
        if (newCapacity < Size) Size = newCapacity;
        internal_resize(newCapacity);
    }

    void shrink_to_fit() {
        if (Capacity == Size) return;
        internal_resize(Size);
    }

#pragma endregion

#pragma region Discard

    [[nodiscard]] constexpr bool can_discard(const size_t count = 1) const noexcept {
        return Size >= count;
    }

    void discard() {
        Size--;
        if constexpr (!std::is_trivially_destructible_v<T>) Data[Size].~T();
    }

    void discard(const Index count) {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index index = Size - count; index < Size; index++)
                Data[Size + index].~T();
        Size -= count;
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const Index count = 1) const noexcept {
        return can_discard(count);
    }

    [[nodiscard]] T& peek() {
        return Data[Size - 1];
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(const Index count = 1) const noexcept {
        return can_discard(count);
    }

    [[nodiscard]] T pop() {
        T value = std::move(Data[--Size]);
        if constexpr (!std::is_trivially_destructible_v<T>) Data[Size].~T();
        return value;
    }

    [[nodiscard]] T& pop_reference() {
        return Data[Size--];
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_push(const Index count = 1) const noexcept {
        return Size + count <= Capacity;
    }

    template <typename... Args>
    void emplace(Args&&... args)
    {
        ::new (&Data[Size++]) T(std::forward<Args>(args)...);
    }

    void push(const T& value)
    {
        ::new (&Data[Size++]) T(value);
    }

    void push(T&& value)
    {
        ::new (&Data[Size++]) T(std::move(value));
    }

#pragma endregion

#pragma endregion

};
