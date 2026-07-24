#pragma once

#pragma region Dependencies

#include "TypeTraits.hpp"

#include <array>
#include <cstddef>

#pragma endregion

template <size_t Capacity>
class StaticStackBuffer {
protected:

    using Address = smallest_uint_t<Capacity>;

    Address Size = 0;
    std::array<uint8_t, Capacity> Data;

public:

#pragma region Methods

    template <typename T> requires (std::is_trivially_copyable_v<T>)
    T operator[](Address index) {
        T value;
        std::memcpy(&value, Data.data() + index - sizeof(T), sizeof(T));
        return value;
    }

#pragma region Discard

    template <typename T>
    [[nodiscard]] constexpr bool can_discard(const size_t count = 1) const noexcept {
        return Size >= count * sizeof(T);
    }

    template <typename T = uint8_t>
    void discard(const Address count) {
        Size -= count * sizeof(T);
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const size_t count = 1) const noexcept {
        return Size + sizeof(T) <= Capacity;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T peek() {
        T value;
        std::memcpy(&value, Data.data() + Size - sizeof(T), sizeof(T));
        return value;
    }

    template <typename T>
    T& peek_reference() {
        return *reinterpret_cast<T*>(Data.data() + Size - sizeof(T));;
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_pop(const size_t count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T pop() {
        Size -= sizeof(T);
        T value;
        std::memcpy(&value, Data.data() + Size, sizeof(T));
        return value;
    }

    template <typename T>
    T& pop_reference() {
        Size -= sizeof(T);
        return *reinterpret_cast<T*>(Data.data() + Size);
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_push(const size_t count = 1) const noexcept {
        return can_peek<T>(count);
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    void push(const T& value) {
        std::memcpy(Data.data() + Size, &value, sizeof(T));
        Size += sizeof(T);
    }

#pragma endregion

#pragma endregion

};
