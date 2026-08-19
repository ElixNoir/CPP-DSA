#pragma once

#pragma region Dependencies

#include "DynamicAoSContainer.hpp"

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class IncrementalDynamicAoSContainer : public DynamicAoSContainer<T, Index, A> {
protected:

    Index Size = 0;

public:

    using Base = DynamicAoSContainer<T, Index, A>;

    IncrementalDynamicAoSContainer(Index initialCapacity) : Base(initialCapacity) {}

    IncrementalDynamicAoSContainer(IncrementalDynamicAoSContainer& other) : Base(other), Size(other.Size) {}

    IncrementalDynamicAoSContainer(IncrementalDynamicAoSContainer&& other) : Base(other), Size(other.Size) {}

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
