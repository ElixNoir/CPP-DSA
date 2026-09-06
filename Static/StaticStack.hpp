#pragma once

#pragma region Dependencies

#include "StaticIncrementalAoSContainer.hpp"

#include <utility>

#pragma endregion

template <typename T, size_t Capacity>
class StaticStack : public StaticIncrementalAoSContainer<T, Capacity> {
public:

    using Base = StaticIncrementalAoSContainer<T, Capacity>;
    using Base::Base::Data;
    using Base::Size;

    using Index = typename Base::Index;

    ~StaticStack() {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index index = 0; index < Size; index++)
                Data[index].~T();
    }

#pragma region Methods

#pragma region Empty

    constexpr void empty() noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index index = 0; index < Size; index++)
                Data[index].~T();
        Base::empty();
    }

#pragma endregion

#pragma region Discard

    [[nodiscard]] constexpr bool can_discard(const size_t count = 1) const noexcept {
        return Size >= count;
    }

    void discard() {
        Size--;
        if constexpr (!std::is_trivially_destructible_v<T>) Data[Size].~T();
    }

    void discard(const Index count) {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index index = Size - count; index < Size; index++)
                Data[index].~T();
        Size -= count;
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const Index count = 1) const noexcept {
        return can_discard(count);
    }

    [[nodiscard]] T& peek() noexcept {
        return Data[Size - 1];
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(const Index count = 1) const noexcept {
        return can_discard(count);
    }

    [[nodiscard]] T pop() {
        T value = std::move(Data[Size]);
        discard();
        return value;
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_push(const Index count = 1) const noexcept {
        return Size + count <= Capacity;
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        ::new (&Data[Size++]) T(std::forward<Args>(args)...);
    }

    void push(const T& value) {
        ::new (&Data[Size++]) T(value);
    }

    void push(T&& value) {
        ::new (&Data[Size++]) T(std::move(value));
    }

#pragma endregion

#pragma region Iterator

    AoSLinearIterator<T> begin() noexcept {
        return AoSLinearIterator<T>(Data);
    }

    AoSLinearIterator<T> end() noexcept {
        return AoSLinearIterator<T>(Data + Size);
    }

    AoSLinearIterator<const T> begin() const noexcept {
        return AoSLinearIterator<const T>(Data);
    }

    AoSLinearIterator<const T> end() const noexcept {
        return AoSLinearIterator<const T>(Data + Size);
    }

#pragma endregion

#pragma endregion

};
