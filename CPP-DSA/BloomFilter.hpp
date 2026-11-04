#pragma once

#include "Hashable.hpp"

/*
template <DSA::concepts::Hashable T>
class BloomFilter {
protected:

    size_t bucket_count = 0;
    size_t hash_count = 0;

    uintmax_t* data;

#pragma region Methods

    // destination must be hash_count in length
    constexpr void hashes(T* const destination, const T& key) const noexcept {
        uintmax_t hash1 = key.hash();
        uintmax_t hash2 = hash1 ^ 0x5bd1e995; // might want one for 64-bit
        for (uintmax_t size_t index = 0; index < hash_count; ++index)
            destination[index] = (hash1 + (index - 1) * hash2) % bucket_count;
    }

#pragma endregion

public:

#pragma region Constructors & Destructors

    BloomFilter(size_t bucketCount, size_t hashCount)
        noexcept (concepts::NothrowAllocatedStorage<Base, T>)
        : bucket_count(bucketCount * BitCount), hash_count(hashCount) {
    } // allocate only bucketCount ??

#pragma endregion

#pragma region Methods

    void add(const T& key) noexcept {
        // alloca?
        hashes(destination, key);
        for (size_t index = 0; index < hash_count; ++index) {
            uintmax_t bucket = destination[index] >> BitShift;
            data[bucket >> BitShift] |= 1 << (bucket & (BitCount - 1));
        }
    }

    [[nodiscard]] constexpr bool contains() const noexcept {
        for (size_t index = 0; index < hash_count; ++index) {
            uintmax_t bucket = destination[index] >> BitShift;
            if (data[bucket] & (1 << (bucket & (BitCount - 1))) == 0)
                return false;
        }
        return true;
    }

#pragma endregion

};
*/