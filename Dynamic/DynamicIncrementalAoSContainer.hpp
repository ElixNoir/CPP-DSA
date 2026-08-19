#pragma once

#pragma region Dependencies

#include "DynamicAoSContainer.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class IncrementalAoSContainer : public AoSContainer<T, Index, A> {
protected:

    Index Size = 0;

public:

    using Base = AoSContainer<T, Index, A>;

    IncrementalAoSContainer(Index initialCapacity) : Base(initialCapacity) {}

    IncrementalAoSContainer(IncrementalAoSContainer& other) : Base(other), Size(other.Size) {}

    IncrementalAoSContainer(IncrementalAoSContainer&& other) : Base(other), Size(other.Size) {}

#pragma region Methods

#pragma region Helpers

    [[nodiscard]] constexpr can_add(Index count = 1) const noexcept {
        return count <= Capacity - Size;
    }

    [[nodiscard]] constexpr can_remove(Index count = 1) const noexcept {
        return Size >= count;
    }

    [[nodiscard]] constexpr is_empty() const noexcept {
        return Size == 0;
    }

    [[nodiscard]] constexpr is_full() const noexcept {
        return Size == Capacity;
    }

#pragma endregion

#pragma region Getters

    [[nodiscard]] constexpr size() const noexcept {
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
