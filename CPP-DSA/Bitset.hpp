#pragma once

#include "BitsetIterator.hpp"
#include "Container.hpp"

namespace DSA {

	template <concepts::StorageConfiguration ContainerConfiguration>
	class Bitset : public ContainerConfiguration::template STRUCTURE<Bitmask<>> {
	public:

		using Base = ContainerConfiguration::template STRUCTURE<Bitmask<>>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitIndex = promoted_t<INDEX>;

		constexpr static Bitmask<>::T BitCount = Bitmask<>::BitCount;
		constexpr static Bitmask<>::T BitShift = Bitmask<>::BitShift;

#pragma region Methods

		constexpr Bitmask<>& get_bitmask_from_bitIndex(BitIndex bitIndex) noexcept {
			return Base::get(bitIndex >> BitShift);
		}

		constexpr const Bitmask<>& get_bitmask_from_bitIndex(BitIndex bitIndex) const noexcept {
			return Base::get(bitIndex >> BitShift);
		}

		constexpr Bitmask<>::T get_bit_at(BitIndex bitIndex) const noexcept {
			return get_bitmask_from_bitIndex(bitIndex).get(bitIndex & (BitCount - 1));
		}

		constexpr void reset_bit_at(BitIndex bitIndex) noexcept {
			get_bitmask_from_bitIndex(bitIndex).reset(bitIndex & (BitCount - 1));
		}

		constexpr void set_bit_at(BitIndex bitIndex) noexcept {
			get_bitmask_from_bitIndex(bitIndex).set(bitIndex & (BitCount - 1));
		}

		template <bool FRONT, bool TRAILING, bool ONES>
		constexpr BitIndex find_first_bit(INDEX capacity = Base::default_capacity()) const noexcept {
			const Bitmask<>::T initialOffset = bitIndex & (BitCount - 1);

			int bitOffset;

			if constexpr (FRONT) {
				BitIndex bitIndex = 0;
				while (bitIndex < capacity) {
					const Bitmask<>& mask = Base::get(bitIndex++);
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

					if (bitOffset != BitCount)
						return bitIndex << BitShift + bitOffset;
				}
			}
			else {
				BitIndex bitIndex = capacity;
				while (bitIndex > 0) {
					const Bitmask<>& mask = Base::get(--bitIndex);
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

					if (bitOffset != BitCount)
						return bitIndex << BitShift + bitOffset;
				}
			}

			return capacity;
		}

		template <bool FRONT, bool TRAILING, bool ONES>
		constexpr BitIndex find_first_bit_after(BitIndex bitIndex, INDEX capacity = Base::default_capacity()) const noexcept {
			const Bitmask<>::T initialOffset = bitIndex & (BitCount - 1);

			int bitOffset;

			bitIndex >>= BitShift; // technically fits in INDEX

			if constexpr (TRAILING) {
				if constexpr (ONES)
					bitOffset = Base::get(bitIndex).index_of_trailing_one(initialOffset);
				else
					bitOffset = Base::get(bitIndex).index_of_trailing_zero(initialOffset);
			}
			else {
				if constexpr (ONES)
					bitOffset = Base::get(bitIndex).index_of_leading_one(initialOffset);
				else
					bitOffset = Base::get(bitIndex).index_of_leading_zero(initialOffset);
			}

			if (bitOffset != BitCount)
				return bitIndex << BitShift + bitOffset;

			if constexpr (FRONT) {
				while (bitIndex < capacity) {
					const Bitmask<>& mask = Base::get(++bitIndex);
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

					if (bitOffset != BitCount)
						return bitIndex << BitShift + bitOffset;
				}
			}
			else {
				while (bitIndex > 0) {
					const Bitmask<>& mask = Base::get(--bitIndex);
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

					if (bitOffset != BitCount)
						return bitIndex << BitShift + bitOffset;
				}
			}

			return capacity;
		}

#pragma endregion

#pragma region Iteration

		template <bool ONES>
		BitsetIterator<INDEX, ONES> begin(INDEX capacity = Base::default_capacity()) noexcept {
			return BitsetIterator<INDEX, ONES>(Base::get_data(), capacity, false);
		}

		template <bool ONES>
		BitsetIterator<INDEX, ONES> end(INDEX capacity = Base::default_capacity()) noexcept {
			return BitsetIterator<INDEX, ONES>(Base::get_data(), capacity, true);
		}

		auto ones(INDEX capacity = Base::default_capacity()) noexcept {
			struct Proxy {
				Bitset* self;

				auto begin() noexcept { return self->template begin<true>(capacity); }
				auto end() noexcept { return self->template end<true>(capacity); }
			};
			return Proxy{ this };
		}

		auto zeros(INDEX capacity = Base::default_capacity()) noexcept {
			struct Proxy {
				Bitset* self;

				auto begin() noexcept { return self->template begin<false>(capacity); }
				auto end() noexcept { return self->template end<false>(capacity); }
			};
			return Proxy{ this };
		}

#pragma endregion

	};

}