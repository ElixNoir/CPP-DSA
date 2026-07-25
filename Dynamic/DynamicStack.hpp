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

public:

    DynamicStack(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<T*>(Alloc.allocate(Capacity * sizeof(T)));
    }

    ~DynamicStack() {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index i = 0; i < Size; ++i)
                Data[i].~T();
        Alloc.deallocate(Data);
    }

#pragma region Methods

    T& operator[](const Index address) {
        return Data[address];
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
        resize(2 * Capacity);
    }

    void reserve(Index newCapacity)
    {
        if (newCapacity > Capacity) resize(newCapacity);
    }

    void resize(Index newCapacity)
    {
        if (newCapacity == Capacity) return;

        if constexpr (ReallocatableAllocator<A> && std::is_trivially_copyable_v<T>)
            Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity * sizeof(T)));
        else {
            T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));

            if (newCapacity < Size) Size = newCapacity;

            for (Index index= 0; index < Size; ++i)
            {
                ::new (&newData[index]) T(std::move(Data[index]));
                if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
            }

            Alloc.deallocate(Data);
            Data = newData;
        }
        
        Capacity = newCapacity;
    }

    void shrink_to_fit() {
        resize(Size);
    }

#pragma endregion

#pragma region Discard

    [[nodiscard]] constexpr bool can_discard(const size_t count = 1) const noexcept {
        return Size >= count;
    }

    void discard(const Index count = 1) {
        Size -= count;
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    [[nodiscard]] T peek() const {
        return Data[Size - 1];
    }

    [[nodiscard]] T& peek_reference() const {
        return Data[Size - 1];
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    [[nodiscard]] T pop() {
        return Data[Size--];
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
