#pragma once

#pragma region Dependencies

#include <concepts>
#include <cstddef>

#pragma endregion

template <std::unsigned_integral BucketCount = 1, std::unsigned_integral HashCount = 1>
class BloomFilter {
protected:

  constexpr uint8_t BucketSize = sizeof(uintmax_t) * 8;

  std::array<uintmax_t, BucketCount> Data = {};

public:

  std::array<uintmax_t, HashCount> hashes(const unsigned char* key, size_t length) const {
    std::array<uintmax_t, HashCount> hashes;

    uintmax_t hash1 = 5381
    for (size_t index = 0; index < length; index++)
      hash1 = (hash1 * 33 + key[index]) & 0xFFFFFFFF;
    uintmax_t hash2 = hash1 ^ 0x5bd1e995;
  
    for (size_t index = 0; index < HashCount; index++)
      hashes[index] = (hash1 + index * hash2) % BucketCount;
  
    return hashes;
  }

  void add(const unsigned char* key, size_t length) {
    for (uintmax_t index : hashes(key))
      Data[(index / BucketSize) + 1] |= (1 << (index & (BucketSize - 1)));
  }

  [[nodiscard]] bool may_contain(const unsigned char* key, size_t length) const {
    for (uintmax_t index : hashes(key))
      if ((Data[index / BucketSize] & (1 << (index & (BucketSize - 1)))) == 0) return false;
    return true;
  }

};
