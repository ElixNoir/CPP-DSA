#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#pragma endregion

template <typename T, Allocator A = DefaultAllocator>
    requires std::is_trivially_copyable_v<T>
class DynamicBitmappedPool {
protected:

    [[no_unique_address]] A Alloc;

    T* Data;
    uintmax_t* FreeMasks;
    size_t Capacity;
    size_t Size = 0;
    size_t Top = 0;

    static constexpr unsigned int Bits = 8 * sizeof(uintmax_t);
    static constexpr unsigned int BitShift = std::bit_width(Bits) - 1;

    void grow(size_t newCapacity) {
        size_t newFreeMasksSize = ((newCapacity + Bits - 1) >> BitShift) * sizeof(uintmax_t);

        if constexpr (ReallocatableAllocator<A>) {
            Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity * sizeof(T)));
            FreeMasks = static_cast<uintmax_t*>(Alloc.reallocate(FreeMasks, newFreeMasksSize));
        }
        else {
            T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
            uintmax_t* newFreeMasks = static_cast<uintmax_t*>(Alloc.allocate(newCapacity * sizeof(T)));
            std::memcpy(newData, Data, Top * sizeof(T));
            std::memcpy(newFreeMasks, FreeMasks, newFreeMasksSize);
            Alloc.deallocate(Data);
            Alloc.deallocate(FreeMasks);
            Data = newData;
            FreeMasks = newFreeMasks;
        }

        Capacity = newCapacity;
    }

public:

    DynamicBitmappedPool(size_t initialCapacity = Bits) : Capacity(initialCapacity) {
        Data = static_cast<T*>(Alloc.allocate(initialCapacity));
        FreeMasks = static_cast<uintmax_t*>(Alloc.allocate((initialCapacity + Bits - 1) >> BitShift));
    }

    ~DynamicBitmappedPool() {
        Alloc.deallocate(Data);
        Alloc.deallocate(FreeMasks);
    }

#pragma region Methods

    [[nodiscard]] T& operator[](size_t index) {
        return Data[index];
    }

#pragma region Getters

    [[nodiscard]] constexpr size_t capacity() const noexcept {
        return Capacity;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        grow(2 * Capacity);
    }

    void reserve(size_t newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

#pragma endregion

#pragma region Allocation and Deallocation

    [[nodiscard]] constexpr bool can_allocate(size_t count = 1) const noexcept {
        return Top + count <= Capacity;
    }

    [[nodiscard]] size_t allocate() {
        size_t freeMaskCount = (Capacity + Bits - 1) >> BitShift;

        Size++;

        for (size_t index = 0; index < freeMaskCount; index++) {
            uintmax_t mask = FreeMasks[index];
            if (mask != std::numeric_limits<uintmax_t>::max()) {
                size_t bit = std::countr_one(mask);
                FreeMasks[index] |= uintmax_t{1} << bit;
                return (index << BitShift) | bit;
            }
        }

        FreeMasks[freeMaskCount] = 1;

        return freeMaskCount << BitShift;
    }

    [[nodiscard]] constexpr bool can_deallocate(size_t count = 1) const noexcept {
        return Size >= count;
    }

    void deallocate(size_t index) {
        Size--;
        FreeMasks[index >> BitShift] &= ~(uintmax_t{1} << (index & (Bits - 1)));
    }

#pragma endregion

#pragma endregion

};
