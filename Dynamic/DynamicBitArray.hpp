#pragma once

#pragma region Dependencies

#include "Bitmask.hpp"
#include "DynamicAoSContainer.hpp"

#pragma endregion

template <std::unsigned_integral T = uintmax_t, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicBitArray : public DynamicAoSContainer<Bitmask<T>, Index, A> {
public:

    using Entry = Bitmask<T>;

    using Base = DynamicAoSContainer<Entry, Index, A>;

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
