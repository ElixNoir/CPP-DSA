#pragma once

#include "BitsetIterator.hpp"

template <concepts::Container<Bitmask<uintmax_t>> CONTAINER>
class Bitset : public CONTAINER {
public:

	using Base = CONTAINER;
	using typename Base::INDEX;
	using typename Base::T;

public:

#pragma region Methods

    constexpr T& get(uintmax_t bitIndex) noexcept {
        return Base::get_data()[bitIndex >> Bitmask<uintmax_t>::BitShift];
    }

	constexpr T& operator[](uintmax_t bitIndex) noexcept {
        return get(bitIndex);
	}

#pragma endregion

#pragma endregion Iteration

	BitsetIterator<INDEX> begin() noexcept {
		return BitsetIterator<INDEX>(Base::get_data(), Base::get_capacity(), false);
	}

	BitsetIterator<INDEX> end() noexcept {
		return BitsetIterator<INDEX>(Base::get_data(), Base::get_capacity(), true);
	}

#pragma endregion

};