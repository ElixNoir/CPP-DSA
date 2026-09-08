#pragma once

#pragma region Dependencies

#include "DynamicIncrementalAoSContainer.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicStack : public DynamicIncrementalAoSContainer<T, Index, A> {
public:

    using Base = DynamicIncrementalAoSContainer<T, Index, A>;
    using Base::Base;

protected:

    using Base::Alloc;
    using Base::Capacity;
    using Base::Data;
    using Base::Size;

public:

    ~DynamicStack() {
        destroy(Data, Size);
    }

#pragma region Methods

#pragma region Memory Management

    void double_capacity() {
        grow(2 * Capacity);
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

    void grow(Index newCapacity) {
        if constexpr (std::is_trivially_copyable_v<T> && ReallocatableAllocator<A>)
            Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity));
        else {
            T* oldData = Data;
            Data = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
            relocate(oldData, Data, Size);
            Alloc.deallocate(oldData);
        }

        Capacity = newCapacity;
    }

    void resize(Index newCapacity) {
        if (newCapacity < Capacity)
            shrink(newCapacity);
        else if (newCapacity > Capacity)
            grow(newCapacity);
    }

    void shrink(Index newCapacity) {
        if (newCapacity < Size) {
            destroy(Data + newCapacity, Size - newCapacity);
            if constexpr (ReallocatableAllocator<A>)
                Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity));
            else {
                T* oldData = Data;
                Data = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
                relocate(oldData, Data, newCapacity);
                Alloc.deallocate(oldData);
            }
        }
        else {
            if constexpr (ReallocatableAllocator<A>)
                Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity));
            else {
                T* oldData = Data;
                Data = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
                relocate(oldData, Data, Size);
                Alloc.deallocate(oldData);
            }
        }

        Capacity = newCapacity;
    }

    void shrink_to_fit() {
        if (Capacity == Size) return;
        shrink(Size);
    }

#pragma endregion

#pragma region Discard

    [[nodiscard]] constexpr bool can_discard(size_t count = 1) const noexcept {
        return Size >= count;
    }

    void discard() {
        destroy(Data + --Size);
    }

    void discard(Index count) {
        Size -= count;
        destroy(Data + Size, count);
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(Index count = 1) const noexcept {
        return can_discard(count);
    }

    [[nodiscard]] T& peek() {
        return Data[Size - 1];
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(Index count = 1) const noexcept {
        return can_discard(count);
    }

    [[nodiscard]] T pop() {
        T value = std::move(Data[--Size]);
        destroy(Data + Size);
        return value;
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_push(Index count = 1) const noexcept {
        return Size + count <= Capacity;
    }

    template <typename... Arguments>
    void emplace(Arguments&&... arguments) {
        construct<T>(Data + Size++, std::forward<Arguments>(arguments)...);
    }

    void push(const T& value) {
        ::new (&Data[Size++]) T(value);
    }

    void push(T&& value) {
        ::new (&Data[Size++]) T(std::move(value));
    }

#pragma endregion

#pragma endregion

};
