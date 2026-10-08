#pragma once

#include "BitsetIterator.hpp"
#include "Container.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class Bitset : public traits::RebindContainer_t<CONTAINER, Bitmask<std::uintmax_t>> {
	public:

		using Base = traits::RebindContainer_t<
			CONTAINER,
			Bitmask<std::uintmax_t>
		>;
		using typename Base::INDEX;
		using typename Base::T;

#pragma region Methods

		constexpr T& get(INDEX index) noexcept {
			return Base::get_data()[index];
		}

		constexpr const T& get(INDEX index) const noexcept {
			return Base::get_data()[index];
		}

		constexpr T& operator[](INDEX index) noexcept {
			return get(index);
		}

		constexpr const T& operator[](INDEX index) const noexcept {
			return get(index);
		}

		constexpr void one() const noexcept {
			std::memset(Base::get_data(), 0xFF, Base::get_capacity());
		}

		constexpr void zero() const noexcept {
			std::memset(Base::get_data(), 0, Base::get_capacity());
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