#pragma once

#pragma region Dependencies

#include "DynamicIncrementalAoSContainer.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
    requires std::is_trivially_copyable_v<T>
class DynamicPool : public DynamicIncrementalAoSContainer<T, Index, A> {
public:

    using Base = DynamicIncrementalAoSContainer<T, Index, A>;

protected:

    using Base::Alloc;
    using Base::Capacity;
    using Base::Data;

    using Base::Size;

    Index* FreeList;
    Index FreeTop = 0;

public:

    DynamicPool(Index initialCapacity) : Base(initialCapacity) {
        FreeList = static_cast<Index*>(Alloc.allocate(Capacity * sizeof(Index)));
    }

    ~DynamicPool() {
        Alloc.deallocate(FreeList);
    }

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size - FreeTop;
    }

    [[nodiscard]] constexpr Index top() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        grow(2 * Capacity);
    }

    void grow(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A>) {
            Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity * sizeof(T)));
            FreeList = static_cast<Index*>(Alloc.reallocate(FreeList, sizeof(Index) * newCapacity));
        }
        else {
            T* oldData = Data;
            Data = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
            relocate(oldData, Data, Size);
            Alloc.deallocate(oldData);

            Index* oldFreeList = FreeList;
            FreeList = static_cast<Index*>(Alloc.allocate(sizeof(Index) * newCapacity));
            relocate(oldFreeList, FreeList, FreeTop);
            Alloc.deallocate(oldFreeList);
        }

        Capacity = newCapacity;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

#pragma endregion

#pragma region Allocation and Deallocation

    [[nodiscard]] constexpr bool can_allocate(const Index count = 1) const noexcept {
        return FreeTop != 0 || Size + count <= Capacity;
    }

    [[nodiscard]] const Index allocate() {
        return FreeTop != 0 ? FreeList[--FreeTop] : Size++;
    }

    void deallocate(Index index) {
        FreeList[FreeTop++] = index;
    }

#pragma endregion

#pragma endregion

};
