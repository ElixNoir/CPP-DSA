#pragma once

#include "BitsetIterator.hpp"
#include "Container.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy>
	class Bitset : public ContainerPolicy<StoragePolicy>::template STRUCTURE<Bitmask<>> { //StoragePolicy::template STRUCTURE<Bitmask<>> {
	public:

		using Base = StoragePolicy::template STRUCTURE<Bitmask<>>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitIndex = promoted_t<INDEX>;

		constexpr static Bitmask<>::T bit_count = Bitmask<>::bit_count;
		constexpr static Bitmask<>::T bit_shift = Bitmask<>::bit_shift;

#pragma region Constructors & Destructors

		constexpr void destroy() noexcept {
			//const INDEX capacity = Base::get_capacity();
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		constexpr Bitmask<>& get_bitmask_from_bitIndex(BitIndex bitIndex) noexcept {
			return Base::get(bitIndex >> bit_shift);
		}

		constexpr const Bitmask<>& get_bitmask_from_bitIndex(BitIndex bitIndex) const noexcept {
			return Base::get(bitIndex >> bit_shift);
		}

		constexpr Bitmask<>::T get_bit_at(BitIndex bitIndex) const noexcept {
			return get_bitmask_from_bitIndex(bitIndex).get(bitIndex & (bit_count - 1));
		}

#pragma endregion

#pragma region Find

		template <bool FRONT, bool TRAILING, bool ONES>
		constexpr BitIndex find_first_bit(INDEX begin = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_first_bit<FRONT, TRAILING, ONES>(begin, capacity);
		}

		template <bool FRONT, bool TRAILING, bool ONES>
		constexpr BitIndex find_first_bit(INDEX begin, INDEX end) const noexcept {
			int bitOffset;
			if constexpr (FRONT) {
				while (begin < end) {
					const Bitmask<>& mask = Base::get(begin++);
					if constexpr (TRAILING) {
						if constexpr (ONES)
							bitOffset = mask.index_of_trailing_one();
						else
							bitOffset = mask.index_of_trailing_zero();
					}
					else {
						if constexpr (ONES)
							bitOffset = mask.index_of_leading_one();
						else
							bitOffset = mask.index_of_leading_zero();
					}

					if (bitOffset != bit_count)
						return begin << bit_shift + bitOffset;
				}
			}
			else {
				while (begin < end) {
					const Bitmask<>& mask = Base::get(--end);
					if constexpr (TRAILING) {
						if constexpr (ONES)
							bitOffset = mask.index_of_trailing_one();
						else
							bitOffset = mask.index_of_trailing_zero();
					}
					else {
						if constexpr (ONES)
							bitOffset = mask.index_of_leading_one();
						else
							bitOffset = mask.index_of_leading_zero();
					}

					if (bitOffset != bit_count)
						return end << bit_shift + bitOffset;
				}
			}

			return end;
		}

		template <bool FRONT, bool TRAILING, bool ONES>
		constexpr BitIndex find_first_bit_after(BitIndex bitBegin) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return find_first_bit_after<FRONT, TRAILING, ONES>(bitBegin, capacity);
		}

		template <bool FRONT, bool TRAILING, bool ONES>
		constexpr BitIndex find_first_bit_after(BitIndex bitBegin, INDEX end) const noexcept {
			const Bitmask<>::T initialOffset = bitBegin & (bit_count - 1);
			const INDEX begin = bitBegin >> bit_shift;

			int bitOffset;

			if constexpr (TRAILING) {
				if constexpr (ONES)
					bitOffset = Base::get(begin).index_of_trailing_one(initialOffset);
				else
					bitOffset = Base::get(begin).index_of_trailing_zero(initialOffset);
			}
			else {
				if constexpr (ONES)
					bitOffset = Base::get(begin).index_of_leading_one(initialOffset);
				else
					bitOffset = Base::get(begin).index_of_leading_zero(initialOffset);
			}

			if (bitOffset != bit_count)
				return begin + bitOffset;

			return find_first_bit<FRONT, TRAILING, ONES>(begin, end);
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