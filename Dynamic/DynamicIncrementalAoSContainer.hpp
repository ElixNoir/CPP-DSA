#pragma once

#pragma region Dependencies

#include "DynamicAoSContainer.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicIncrementalAoSContainer : public DynamicAoSContainer<T, Index, A> {
protected:

    Index Size = 0;

public:

    using Base = DynamicAoSContainer<T, Index, A>;

    DynamicIncrementalAoSContainer(Index initialCapacity) : Base(initialCapacity) {}

    DynamicIncrementalAoSContainer(DynamicIncrementalAoSContainer& other) : Base(other), Size(other.Size) {}

    DynamicIncrementalAoSContainer(DynamicIncrementalAoSContainer&& other) : Base(other), Size(other.Size) {}

#pragma region Methods

#pragma region Helpers

    [[nodiscard]] constexpr bool can_add(Index count = 1) const noexcept {
        return count <= Capacity - Size;
    }

    [[nodiscard]] constexpr bool can_remove(Index count = 1) const noexcept {
        return Size >= count;
    }

    [[nodiscard]] constexpr bool is_empty() const noexcept {
        return Size == 0;
    }

    [[nodiscard]] constexpr bool is_full() const noexcept {
        return Size == Capacity;
    }

#pragma endregion

#pragma region Getters

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void shrink_to_fit() {
        shrink(Size);
    }

#pragma endregion

#pragma endregion

};
