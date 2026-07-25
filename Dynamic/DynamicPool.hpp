#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
    requires std::is_trivially_copyable_v<T>
class DynamicPool {
protected:

    [[no_unique_address]] A Alloc;

    T* Data;
    Index* FreeList;
    Index Top = 0;
    Index FreeTop = 0;
    Index Capacity;

    void grow(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A>) {
            Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity * sizeof(T)));
            FreeList = static_cast<Index*>(Alloc.reallocate(FreeList, newCapacity * sizeof(T)));
        } else {
            T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
            Index* newFreeList = static_cast<Index*>(Alloc.allocate(newCapacity * sizeof(T)));
            std::memcpy(newData, Data, Top * sizeof(T));
            std::memcpy(newData, Data, FreeTop * sizeof(T));
            Alloc.deallocate(Data);
            Alloc.deallocate(FreeList);
            Data = newData;
            FreeList = newFreeList;
        }

        Capacity = newCapacity;
    }

public:

    DynamicPool(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<T*>(Alloc.allocate(Capacity * sizeof(T)));
        FreeList = static_cast<Index*>(Alloc.allocate(Capacity * sizeof(Index)));
    }

    ~DynamicPool() {
        Alloc.deallocate(Data);
        Alloc.deallocate(FreeList);
    }

#pragma region Methods

    [[nodiscard]] T& operator[](Index index) {
        return Data[index];
    }

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Top - FreeTop;
    }

    [[nodiscard]] constexpr Index top() const noexcept {
        return Top;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        grow(2 * Capacity);
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

#pragma endregion

#pragma region Allocation and Deallocation

    [[nodiscard]] constexpr bool can_allocate(const Index count = 1) const noexcept {
        return FreeTop != 0 || Top + count <= Capacity;
    }

    [[nodiscard]] const Index allocate() {
        return FreeTop != 0 ? FreeList[--FreeTop] : Top++;
    }

    void deallocate(Index index) {
        FreeList[FreeTop++] = index;
    }

#pragma endregion

#pragma endregion

};
