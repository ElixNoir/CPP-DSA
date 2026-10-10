#pragma once

#include "BitsetIterator.hpp"
#include "Container.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class Bitset : public traits::container_replace_first_arg_t<CONTAINER, Bitmask<>> {
	public:

		using Base = traits::container_replace_first_arg_t<CONTAINER, Bitmask<>>;
		using typename Base::INDEX;
		using typename Base::T;

#pragma region Methods

		constexpr bool get_bit_at(INDEX index) const noexcept {
			return Base::get(index >> Bitmask<uintmax_t>::BitShift).get(index & (Bitmask<uintmax_t>::BitCount - 1));
		}

		constexpr void set_bit_at(INDEX index) noexcept {
			Base::get(index >> Bitmask<uintmax_t>::BitShift).set(index & (Bitmask<uintmax_t>::BitCount - 1));
		}

#pragma endregion

#pragma endregion Iteration

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
				decltype(this) self;

				auto begin() noexcept { return self->template begin<true>(); }
				auto end() noexcept { return self->template end<true>(); }
			};
			return Proxy{ this };
		}

		auto zeros() noexcept {
			struct Proxy {
				decltype(this) self;

				auto begin() noexcept { return self->template begin<false>(); }
				auto end() noexcept { return self->template end<false>(); }
			};
			return Proxy{ this };
		}

#pragma endregion

	};

}