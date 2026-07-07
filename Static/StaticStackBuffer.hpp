#pragma once

#pragma region Dependencies

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#pragma endregion

template <size_t Capacity = 0x100>
class StaticStackBuffer {
protected:

    using Address = std::conditional_t<
        Capacity <= 0x100, uint8_t,
        std::conditional_t<
            Capacity <= 0x10000, uint16_t,
            std::conditional_t<
                Capacity <= 0x100000000ULL, uint32_t,
                uint64_t
            >
        >
    >;

    std::array<uint8_t, Capacity> Data;
    Address Size = 0;

public:

    #pragma region Methods

    template <typename T>
    bool can_pop<T>(const size_t count = 1) const noexcept {
        return Size - count * sizeof(T) >= 0;
    }

    template <typename T>
    bool can_push<T>() const noexcept {
        return Size + sizeof(T) <= Capacity;
    }

    template <typename T>
    T& unsafe_peek<T>() const {
        return *reinterpret_cast<T*>(Data.data() + Size - sizeof(T));
    }

    template <typename T>
    T unsafe_pop<T>() {
        Size -= sizeof(T);
        return *reinterpret_cast<T*>(Data.data() + Size);
    }

    template <typename T>
    T& unsafe_pop_reference<T>() {
        Size -= sizeof(T);
        return *reinterpret_cast<T*>(Data.data() + Size);
    }

    template <typename T>
    void unsafe_push(T& value) {
        std::memcpy(Data.data() + Size, &value, sizeof(T));
        Size += sizeof(T);
    }

    #pragma endregion

};
