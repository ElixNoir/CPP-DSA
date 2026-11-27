#pragma once

#include "BitsetIterator.hpp"
#include "Container.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, std::unsigned_integral T_ = uintmax_t>
	class Bitset : public ContainerPolicy::template STRUCTURE<Bitmask<T_>> {
	public:

		using Base = ContainerPolicy::template STRUCTURE<Bitmask<T_>>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitIndex = promoted_t<INDEX>;

		constexpr static T bit_count = Bitmask<T>::bit_count;
		constexpr static T bit_shift = Bitmask<T>::bit_shift;

#pragma region Methods

#pragma region Getters

		constexpr Bitmask<T>& get_bitmask_from_bitIndex(const BitIndex bitIndex) noexcept {
			return Base::get(bitIndex >> bit_shift);
		}

		constexpr const Bitmask<T>& get_bitmask_from_bitIndex(const BitIndex bitIndex) const noexcept {
			return Base::get(bitIndex >> bit_shift);
		}

		constexpr Bitmask<T>::T get_bit_at(const BitIndex bitIndex) const noexcept {
			return get_bitmask_from_bitIndex(bitIndex).get(bitIndex & (bit_count - 1));
		}

#pragma endregion

#pragma region Find

		template <Direction DIRECTION = Direction::Forward, bool ONES>
		constexpr BitIndex find_bit(const INDEX begin = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_bit<DIRECTION, ONES>(begin, capacity);
		}

		template <Direction DIRECTION = Direction::Forward, bool ONES>
		constexpr BitIndex find_bit(const INDEX begin, const INDEX end) const noexcept {
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
		constexpr BitIndex find_first_bit_at_or_after(const BitIndex bitBegin) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_first_bit_at_or_after<ONES>(bitBegin, capacity);
		}

		template <bool ONES>
		constexpr BitIndex find_first_bit_at_or_after(const BitIndex bitBegin, const INDEX end) const noexcept {
			const T initialOffset = bitBegin & (bit_count - 1);
			const INDEX begin = bitBegin >> bit_shift;
			const T bitmask = Base::get(begin);
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
		constexpr BitIndex find_last_bit_at_or_before(const BitIndex bitEnd) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_last_bit_at_or_before<ONES>(0, bitEnd);
		}

		template <bool ONES>
		constexpr BitIndex find_last_bit_at_or_before(const INDEX begin, const BitIndex bitEnd) const noexcept {
			const T initialOffset = bitEnd & (bit_count - 1);
			const INDEX end = bitEnd >> bit_shift;
			const T bitmask = Base::get(end);
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

		constexpr void reset_bit_at(const BitIndex bitIndex) noexcept {
			get_bitmask_from_bitIndex(bitIndex).reset(bitIndex & (bit_count - 1));
		}

		constexpr void set_bit_at(const BitIndex bitIndex) noexcept {
			get_bitmask_from_bitIndex(bitIndex).set(bitIndex & (bit_count - 1));
		}

#pragma endregion

#pragma region Iteration

		template <bool ONES>
		constexpr auto begin() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return BitsetIterator<INDEX, ONES, T>(Base::get_data(), capacity, false);
		}

		template <bool ONES>
		constexpr auto end() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return BitsetIterator<INDEX, ONES, T>(Base::get_data(), capacity, true);
		}

		constexpr auto ones() const noexcept {
			struct Proxy {
				Bitset* self;

				auto begin() const noexcept { return self->begin<true>(); }
				auto end() const noexcept { return self->end<true>(); }
			};
			return Proxy{ this };
		}

		constexpr auto zeros() const noexcept {
			struct Proxy {
				Bitset* self;

				auto begin() const noexcept { return self->begin<false>(); }
				auto end() const noexcept { return self->end<false>(); }
			};
			return Proxy{ this };
		}

#pragma endregion

#pragma endregion

	};

}