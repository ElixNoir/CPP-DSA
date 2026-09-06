#pragma once

#pragma region Dependencies

#include "Bitmask.hpp"
#include "StaticAoSContainer.hpp"

#pragma endregion

template <size_t Capacity, std::unsigned_integral T = uintmax_t>
class StaticBitArray : public StaticAoSContainer<Bitmask<T>, Capacity> {
public:

    using Entry = Bitmask<T>;

    using Base = StaticAoSContainer<Entry, Capacity>;

protected:

    using Base::Data;

    constexpr static unsigned int IndexBitMask = 8 * sizeof(Entry) - 1;

public:

    using Base::Base;

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr size_t index_from(size_t bitIndex) const noexcept {
        return bitIndex >> std::bit_width(IndexBitMask);
    }

    [[nodiscard]] constexpr Entry& bitmask_at(size_t bitIndex) noexcept {
        return Data[index_from(bitIndex)];
    }

    [[nodiscard]] constexpr const Entry& bitmask_at(size_t bitIndex) const noexcept {
        return bitmask_at(bitIndex);
    }

#pragma endregion

#pragma endregion

};
