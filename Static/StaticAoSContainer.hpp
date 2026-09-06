#pragma once

#pragma region Dependencies

#include "AoSLinearIterator.hpp"
#include "TypeTraits.hpp"

#include <algorithm>
#include <cstddef>

#pragma endregion

template <typename T, size_t Capacity>
class StaticAoSContainer {
public:

    using Index = smallest_uint_t<Capacity>;

protected:

    T Data[Capacity];

public:

    StaticAoSContainer() = default;

    StaticAoSContainer(const StaticAoSContainer& other) {
        std::copy(
            other.Data,
            other.Data + Capacity,
            Data);
    }

    StaticAoSContainer(StaticAoSContainer&& other) : Data(other.Data) {
        std::move(
            other.Data,
            other.Data + Capacity,
            Data);
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
