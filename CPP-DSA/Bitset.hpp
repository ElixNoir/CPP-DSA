#pragma once

#include "BitsetIterator.hpp"
#include "Container.hpp"

namespace DSA {

	template <typename ContainerConfiguration>
	class Bitset : public ContainerConfiguration::template CONTAINER<Bitmask<>> {
	public:

		using Base = ContainerConfiguration::template CONTAINER<Bitmask<>>;
		using typename Base::INDEX;
		using typename Base::T;

#pragma region Methods

		constexpr Bitmask<>& get_bitmask_from_bitIndex(INDEX bitIndex) noexcept {
			return Base::get(bitIndex >> Bitmask<>::BitShift);
		}

		constexpr const Bitmask<>& get_bitmask_from_bitIndex(INDEX bitIndex) const noexcept {
			return Base::get(bitIndex >> Bitmask<>::BitShift);
		}

		constexpr bool get_bit_at(INDEX index) const noexcept {
			return get_bitmask_from_bitIndex(index).get(index & (Bitmask<>::BitCount - 1));
		}

		constexpr void reset_bit_at(INDEX index) noexcept {
			get_bitmask_from_bitIndex(index).reset(index & (Bitmask<>::BitCount - 1));
		}

		constexpr void set_bit_at(INDEX index) noexcept {
			get_bitmask_from_bitIndex(index).set(index & (Bitmask<>::BitCount - 1));
		}

		constexpr INDEX first_leading_one(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index < capacity) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_leading_one();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index += Bitmask<>::BitCount;
			}
			return capacity;
		}

		constexpr INDEX first_leading_zero(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index < capacity) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_leading_zero();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index += Bitmask<>::BitCount;
			}
			return capacity;
		}

		constexpr INDEX first_trailing_one(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index < capacity) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_trailing_one();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index += Bitmask<>::BitCount;
			}
			return capacity;
		}

		constexpr INDEX first_trailing_zero(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index < capacity) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_trailing_zero();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index += Bitmask<>::BitCount;
			}
			return capacity;
		}

		constexpr INDEX last_leading_one(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index > Bitmask<>::BitCount) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_leading_one();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index -= Bitmask<>::BitCount;
			}
			return capacity;
		}

		constexpr INDEX last_leading_zero(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index > Bitmask<>::BitCount) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_leading_zero();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index -= Bitmask<>::BitCount;
			}
			return capacity;
		}

		constexpr INDEX last_trailing_one(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index > Bitmask<>::BitCount) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_trailing_one();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index -= Bitmask<>::BitCount;
			}
			return capacity;
		}

		constexpr INDEX last_trailing_zero(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index > Bitmask<>::BitCount) {
				int bitIndex = get_bitmask_from_bitIndex(index).index_of_trailing_zero();
				if (bitIndex != Bitmask<>::BitCount)
					return index + bitIndex;
				index -= Bitmask<>::BitCount;
			}
			return capacity;
		}

#pragma endregion

#pragma region Iteration

		template <bool ONES>
		BitsetIterator<INDEX, ONES> begin() noexcept {
			return BitsetIterator<INDEX, ONES>(Base::get_data(), Base::get_capacity(), false);
		}

		template <bool ONES>
		BitsetIterator<INDEX, ONES> end() noexcept {
			return BitsetIterator<INDEX, ONES>(Base::get_data(), Base::get_capacity(), true);
		}

		auto ones() noexcept {
			struct Proxy {
				Bitset* self;

				auto begin() noexcept { return self->template begin<true>(); }
				auto end() noexcept { return self->template end<true>(); }
			};
			return Proxy{ this };
		}

		auto zeros() noexcept {
			struct Proxy {
				Bitset* self;

				auto begin() noexcept { return self->template begin<false>(); }
				auto end() noexcept { return self->template end<false>(); }
			};
			return Proxy{ this };
		}

#pragma endregion

	};

}