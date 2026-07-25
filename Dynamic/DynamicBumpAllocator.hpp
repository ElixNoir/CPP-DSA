#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicBumpAllocator {
protected:

    uint8_t* Data;
    Index Size = 0;
    Index Capacity;

    void internal_resize(Index newCapacity) {
        if constexpr (ReallocatableAllocator<T>)
            Data = static_cast<uint8_t*>(Alloc.reallocate(Data, newCapacity));
        else {
            uint8_t* newData = static_cast<uint8_t*>(Alloc.allocate(newCapacity));
            std::memcpy(newData, Data, Capacity);
        
            Alloc.deallocate(Data);
        
            Capacity = newCapacity;
            Data = newData;
        }
    }

public:

    DynamicBumpAllocator(Index initialCapacity = 0x100): Capacity(initialCapacity) {
        Data = static_cast<uint8_t*>(Alloc.allocate(initialCapacity));
    }

    ~DynamicBumpAllocator() {
        Alloc.deallocate(Data);
    }

#pragma region Methods

    template <typename T>
    T& operator[](Index index) {
        return *reinterpret_cast<T*>(Data + index);
    }

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        internal_resize(2 * Capacity);
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) internal_resize(newCapacity);
    }

    void resize(Index newCapacity) {
        if (newCapacity < Size) Size = newCapacity;
        internal_resize(newCapacity);
    }

    void shrink_to_fit() {
        internal_resize(Size);
    }

#pragma endregion

#pragma Allocation

    template <typename T>
    [[nodiscard]] constexpr bool can_allocate<T>(size_t count = 1) const noexcept {
        return Size + sizeof(T) * count <= Capacity;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] Index allocate<T>(size_t count = 1) {
        Index index = Size;
        Size += sizeof(T) * count;
        return index;
    }

    [[nodiscard]] void* allocate(size_t size) {
        Index index = Size;
        Size += size;
        return static_cast<void*>(Data + index);
    }

#pragma endregion

#pragma endregion

};
