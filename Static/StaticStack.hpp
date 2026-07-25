#pragma once

#pragma region Dependencies

#include "TypeTraits.hpp"

#include <array>
#include <cstddef>
#include <utility>

#pragma endregion

template <typename T, size_t Capacity>
class StaticStack {
protected:

    using Index = smallest_uint_t<Capacity>;

    Index Size = 0;
    std::array<T, Capacity> Data;

public:

    ~StaticStack() {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index index = 0; index < Size; index++)
                Data[index].~T();
    }

#pragma region Methods

    [[nodiscard]] T& operator[](const Index address) {
        return Data[address];
    }

#pragma region Getters

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr T* data() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Discard

    [[nodiscard]] constexpr bool can_discard(const size_t count = 1) const noexcept {
        return Size >= count;
    }

    void discard(const Index count = 1) {
        Size -= count;
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    [[nodiscard]] T& peek() const {
        return Data[Size - 1];
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    [[nodiscard]] T pop() {
        T value = std::move(Data[--Size]);
        if constexpr (!std::is_trivially_destructible_v<T>) Data[Size].~T();
        return value;
    }

    [[nodiscard]] T& pop_reference() {
        return Data[Size--];
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_push(const Index count = 1) const noexcept {
        return Size + count <= Capacity;
    }

    template <typename... Args>
    void emplace(Args&&... args)
    {
        ::new (&Data[Size++]) T(std::forward<Args>(args)...);
    }

    void push(const T& value)
    {
        ::new (&Data[Size++]) T(value);
    }

    void push(T&& value)
    {
        ::new (&Data[Size++]) T(std::move(value));
    }

#pragma endregion

#pragma endregion

};
