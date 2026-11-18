#pragma once

#include "BitsetIterator.hpp"
#include "Container.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, std::unsigned_integral _T = uintmax_t>
	class Bitset : public ContainerPolicy::template STRUCTURE<Bitmask<_T>> {
	public:

		using Base = ContainerPolicy::template STRUCTURE<Bitmask<_T>>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitIndex = promoted_t<INDEX>;

		constexpr static _T bit_count = Bitmask<T>::bit_count;
		constexpr static _T bit_shift = Bitmask<T>::bit_shift;

#pragma region Methods

#pragma region Getters

		constexpr Bitmask<T>& get_bitmask_from_bitIndex(BitIndex bitIndex) noexcept {
			return Base::get(bitIndex >> bit_shift);
		}

		constexpr const Bitmask<T>& get_bitmask_from_bitIndex(BitIndex bitIndex) const noexcept {
			return Base::get(bitIndex >> bit_shift);
		}

		constexpr Bitmask<T>::T get_bit_at(BitIndex bitIndex) const noexcept {
			return get_bitmask_from_bitIndex(bitIndex).get(bitIndex & (bit_count - 1));
		}

#pragma endregion

#pragma region Find

		template <Direction DIRECTION = Direction::Forward, bool ONES>
		constexpr BitIndex find_bit(INDEX begin = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_bit<DIRECTION, ONES>(begin, capacity);
		}

		template <Direction DIRECTION = Direction::Forward, bool ONES>
		constexpr BitIndex find_bit(INDEX begin, INDEX end) const noexcept {
			int bitOffset;
			if constexpr (DIRECTION == Direction::Forward) {
				while (begin < end) {
					const Bitmask<T>& mask = Base::get(begin);
					if constexpr (ONES)
						bitOffset = mask.index_of_trailing_one();
					else
						bitOffset = mask.index_of_trailing_zero();
					if (bitOffset != bit_count)
						return (begin << bit_shift) + bitOffset;
					++begin;
				}
			}
			else {
				while (begin < end) {
					const Bitmask<T>& mask = Base::get(--end);
					if constexpr (ONES)
						bitOffset = mask.index_of_leading_one();
					else
						bitOffset = mask.index_of_leading_zero();
					if (bitOffset != 0)
						return (end << bit_shift) + bitOffset;
				}
			}

			return end;
		}

		template <bool ONES>
		constexpr BitIndex find_first_bit() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_first_bit_at_or_after<ONES>(0, capacity);
		}

		template <bool ONES>
		constexpr BitIndex find_first_bit_at_or_after(BitIndex bitBegin) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_first_bit_at_or_after<ONES>(bitBegin, capacity);
		}

		template <bool ONES>
		constexpr BitIndex find_first_bit_at_or_after(BitIndex bitBegin, INDEX end) const noexcept {
			const Bitmask<T>::T initialOffset = bitBegin & (bit_count - 1);
			const INDEX begin = bitBegin >> bit_shift;
			const Bitmask<T>::T bitmask = Base::get(begin);
			int bitOffset;
			if constexpr (ONES)
				bitOffset = bitmask.index_of_trailing_one(initialOffset);
			else
				bitOffset = bitmask.index_of_trailing_zero(initialOffset);

			if (bitOffset != bit_count)
				return (begin << bit_shift) + bitOffset;

			return find_bit<Direction::Forward, ONES>(begin + 1, end);
		}

		template <bool ONES>
		constexpr BitIndex find_last_bit() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_bit<Direction::Backward, ONES>(0, capacity);
		}

		template <bool ONES>
		constexpr BitIndex find_last_bit_at_or_before(BitIndex bitEnd) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_last_bit_at_or_before<ONES>(0, bitEnd);
		}

		template <bool ONES>
		constexpr BitIndex find_last_bit_at_or_before(INDEX begin, BitIndex bitEnd) const noexcept {
			const Bitmask<T>::T initialOffset = bitEnd & (bit_count - 1);
			const INDEX end = bitEnd >> bit_shift;
			const Bitmask<T>::T bitmask = Base::get(end);
			int bitOffset;
			if constexpr (ONES)
				bitOffset = bitmask.index_of_leading_one(initialOffset);
			else
				bitOffset = bitmask.index_of_leading_zero(initialOffset);
			if (bitOffset != bit_count)
				return (end << bit_shift) + bitOffset;

			return find_bit<Direction::Backward, ONES>(begin, end);
		}

#pragma endregion

#pragma region Setters

		constexpr void reset_bit_at(BitIndex bitIndex) noexcept {
			get_bitmask_from_bitIndex(bitIndex).reset(bitIndex & (bit_count - 1));
		}

		constexpr void set_bit_at(BitIndex bitIndex) noexcept {
			get_bitmask_from_bitIndex(bitIndex).set(bitIndex & (bit_count - 1));
		}

#pragma endregion

#pragma region Iteration

		template <bool ONES>
		constexpr auto begin() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return BitsetIterator<INDEX, ONES>(Base::get_data(), capacity, false);
		}

		template <bool ONES>
		constexpr auto end() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return BitsetIterator<INDEX, ONES>(Base::get_data(), capacity, true);
		}

		constexpr auto ones() const noexcept {
			const INDEX capacity = Base::get_capacity();
			struct Proxy {
				Bitset* self;

				auto begin() noexcept { return self->template begin<true>(capacity); }
				auto end() noexcept { return self->template end<true>(capacity); }
			};
			return Proxy{ this };
		}

		constexpr auto zeros() const noexcept {
			const INDEX capacity = Base::get_capacity();
			struct Proxy {
				Bitset* self;

				auto begin() noexcept { return self->template begin<false>(capacity); }
				auto end() noexcept { return self->template end<false>(capacity); }
			};
			return Proxy{ this };
		}

#pragma endregion

#pragma endregion

	};

}