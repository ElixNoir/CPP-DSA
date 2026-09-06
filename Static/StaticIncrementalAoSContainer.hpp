#pragma once

#pragma region Dependencies

#include "StaticAoSContainer.hpp"

#pragma endregion

template <typename T, size_t Capacity>
class StaticIncrementalAoSContainer : public StaticAoSContainer<T, Capacity> {
public:

    using Base = StaticAoSContainer<T, Capacity>;

    using Index = typename Base::Index;

protected:

    using Base::Data;

    Index Size = 0;

public:

    constexpr StaticIncrementalAoSContainer() = default;

    constexpr StaticIncrementalAoSContainer(const StaticIncrementalAoSContainer& other) : Base(other), Size(other.Size) {}

    constexpr StaticIncrementalAoSContainer(StaticIncrementalAoSContainer&& other) : Base(std::move(other)), Size(other.Size) {}

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

#pragma region Miscellaneous

    constexpr void empty() noexcept {
        Size = 0;
    }

#pragma endregion

#pragma endregion

};
