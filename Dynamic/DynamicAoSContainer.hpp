#pragma once

#pragma region Dependencies

#include "AoSLinearIterator.hpp"
#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"
#include "Memory.hpp"

#include <algorithm>
#include <cstddef>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicAoSContainer {
protected:

    [[no_unique_address]] A Alloc;

    T* Data;
    Index Capacity;

public:

    DynamicAoSContainer(Index initialCapacity) : Data(static_cast<T*>(Alloc.allocate(sizeof(T)* initialCapacity))), Capacity(initialCapacity) {}

    ~DynamicAoSContainer() {
        Alloc.deallocate(Data);
    }

#pragma region Methods

#pragma region Operators

    [[nodiscard]] T& operator[](const Index index) noexcept {
        return Data[index];
    }

    [[nodiscard]] const T& operator[](const Index index) const noexcept {
        return Data[index];
    }

#pragma endregion

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr T* data() noexcept {
        return Data;
    }

    [[nodiscard]] constexpr const T* data() const noexcept {
        return Data;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        resize(2 * Capacity);
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity)
            resize(newCapacity);
    }

    void resize(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A>)
            Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity));
        else {
            T* oldData = Data;
            Data = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
            Alloc.deallocate(oldData);
        }
    }

#pragma endregion

#pragma region Iteration

    AoSLinearIterator<T> begin() noexcept {
        return AoSLinearIterator<T>(Data);
    }

    AoSLinearIterator<T> end() noexcept {
        return AoSLinearIterator<T>(Data + Capacity);
    }

    AoSLinearIterator<const T> begin() const noexcept {
        return AoSLinearIterator<const T>(Data);
    }

    AoSLinearIterator<const T> end() const noexcept {
        return AoSLinearIterator<const T>(Data + Capacity);
    }

#pragma endregion

#pragma endregion

};
