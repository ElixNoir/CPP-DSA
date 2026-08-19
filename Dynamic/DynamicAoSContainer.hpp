#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>
#include <cstring>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicAoSContainer {
protected:

    [[no_unique_address]] A Alloc;

    T* Data;
    Index Capacity;

public:

    DynamicAoSContainer(Index initialCapacity) {
        Data = static_cast<T*>(Alloc.allocate(sizeof(T) * initialCapacity));
    }

    DynamicAoSContainer(DynamicAoSContainer& other) noexcept : Capacity(other.Capacity) {
        Data = static_cast<T*>(Alloc.allocate(sizeof(T) * initialCapacity));
        std::memcpy(Data, other.Data, sizeof(T) * other.Capacity);
    }

    DynamicAoSContainer(DynamicAoSContainer&& other) noexcept : Data(other.Data), Capacity(other.Capacity) {
        other.Data = nullptr;
    }

    ~DynamicAoSContainer() {
        Alloc.deallocate(Data);
    }

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr data() const noexcept {
        return Data;
    }

#pragma endregion

#pragma region Memory Management

// Eventually, there needs to be a way to track non-trivial T.

#pragma region Grow

    void double_capacity() {
        grow(2 * Capacity);
    }

    void grow(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A>)
            Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity));
        else {
            T* newData = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
            std::memcpy(newData, Data, sizeof(T) * Capacity);
            Alloc.deallocate(Data);
            Data = newData;
        }
        
        Capacity = newCapacity;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

#pragma endregion

#pragma region Shrink

    void resize(Index newCapacity) {
        if (newCapacity < Capacity) shrink(newCapacity);
        else if (newCapacity > Capacity) grow(newCapacity);
    }
    
    void shrink(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A>)
            Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity));
        else {
            T* newData = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
            std::memcpy(newData, Data, sizeof(T) * newCapacity);
            Alloc.deallocate(Data);
            Data = newData;
        }
        
        Capacity = newCapacity;
    }

#pragma endregion

#pragma endregion

#pragma endregion

};
