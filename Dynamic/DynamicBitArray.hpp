#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"
#include "StaticBitArray.hpp"

#pragma endregion

template <std::unsigned_integral T = uintmax_t, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicBitArray : public DynamicArray<StaticBitArray<T>, Index, A> {
public:

  using Entry = StaticBitArray<T>;
  using Base = DynamicArray<Entry, Index, A>;

protected:

  [[nodiscard]] constexpr size_t IndexBitMask = 8 * sizeof(size_t) - 1;

public:

  using Base::Base;

#pragma region Methods

#pragma region Getters

  [[nodiscard]] constexpr Entry bit_array_at(size_t bitIndex) const noexcept {
    return Data[index >> std::bit_width(IndexBitMask)];
  }

#pragma endregion

#pragma endregion

};
