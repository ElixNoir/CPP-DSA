#pragma once

#include "Comparable.hpp"
#include "Memory.hpp"

namespace Algorithms {

	namespace Aggregation {

		const static uint64_t Factorials[] = {
			1, 1, 2, 6, 24, 120, 720, 5040, 40320,
			362880, 3628800, 39916800, 479001600,
			6227020800, 87178291200, 1307674368000,
			20922789888000, 355687428096000, 6402373705728000
		};

		template <DSA::concepts::Comparable T>
		constexpr static T& maximum(T& a, T& b)
			noexcept(noexcept(a > b))
		{
			return a > b ? a : b;
		}

		template <DSA::concepts::Comparable T>
		constexpr static const T& maximum(const T& a, const T& b)
			noexcept(noexcept(a > b))
		{
			return a > b ? a : b;
		}

		template <DSA::concepts::Comparable T>
		constexpr static T& minimum(T& a, T& b)
			noexcept(noexcept(a < b))
		{
			return a < b ? a : b;
		}

		template <DSA::concepts::Comparable T>
		constexpr static const T& minimum(const T& a, const T& b)
			noexcept(noexcept(a < b))
		{
			return a < b ? a : b;
		}

		template <DSA::concepts::Comparable T>
		constexpr static uint64_t permutation_identity_of(const T* const initial, uint8_t length, const T* const target)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
		{
			T available[18];
			Memory::copy_construct_range_forward(available, available + 18, initial);
			uint8_t top = length;
			uint64_t identity = 0;

			for (uint8_t index = 0; index < length; ++index) {
				const auto& value = target[index];
				uint8_t position = 0;
				while (position < top && value != available[position])
					++position;
				identity += position * Factorials[length - index];
				while (++position < top)
					available[position - 1] = available[position];
				--top;
			}

			Memory::destroy_range_forward(available, available + 18);

			return identity;
		}

		template <DSA::concepts::Comparable T>
		constexpr static void permute(const T* const data, uint8_t length, uint64_t identity)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
		{
			T available[18];
			Memory::copy_construct_range_forward(available, available + 18, data);

			for (uint8_t index = 0; index < length; ++index) {
				const uint64_t factorial = Factorials[length - index];
				data[index] = available[identity / factorial + index];
				identity %= factorial;
			}

			Memory::destroy_range_forward(available, available + 18);
		}

	}

}