#pragma once

#include "Bitmask.hpp"

#include <concepts>
#include <cstddef>
#include <iterator>

template <std::unsigned_integral INDEX>
class BitsetIterator {
public:

    using MASK = Bitmask<uintmax_t>;

    using value_type = INDEX;
    using difference_type = std::ptrdiff_t;
    using reference = INDEX;
    using pointer = void;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = std::forward_iterator_tag;

protected:

    const MASK* address = nullptr;
    INDEX mask = 0;
    INDEX bit = MASK::BitCount;
    INDEX maskCount = 0;

    constexpr void seek() {
        while (mask < maskCount) {
            const int found = address[mask].index_of_trailing_one(
                static_cast<int>(bit));

            if (found != MASK::BitCount) {
                bit = static_cast<INDEX>(found);
                return;
            }

            ++mask;
            bit = 0;
        }

        // End sentinel.
        mask = maskCount;
        bit = MASK::BitCount;
    }

public:

    constexpr BitsetIterator() = default;

    constexpr BitsetIterator(
        const MASK* address,
        INDEX maskCount,
        bool end
    ) :
        address(address),
        mask(0),
        bit(MASK::BitCount),
        maskCount(maskCount)
    {
        if (!end)
            seek();
        else
            mask = maskCount;
    }

    constexpr INDEX operator*() const noexcept {
        return mask * MASK::BitCount + bit;
    }

    constexpr BitsetIterator& operator++() {
        ++bit;
        seek();
        return *this;
    }

    constexpr BitsetIterator operator++(int) {
        auto copy = *this;
        ++*this;
        return copy;
    }

    constexpr bool operator==(
        const BitsetIterator& other
    ) const noexcept {
        return address == other.address
            && mask == other.mask
            && bit == other.bit;
    }

    constexpr bool operator!=(
        const BitsetIterator& other
    ) const noexcept {
        return !(*this == other);
    }

};