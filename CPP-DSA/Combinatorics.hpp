#pragma once

#include "Math.hpp"
#include "Comparable.hpp"
#include "Constants.hpp"
#include "Memory.hpp"

namespace Algorithms {

	namespace Combinatorics {

		constexpr static double fibonacci(uint32_t nth) noexcept {
			return Math::power(Constants::phi, nth) / Constants::square_root_of_5 + .5;
		}

		template <typename T>
		constexpr static double ln_abs_gamma(T data) noexcept {
			if constexpr (std::same_as<T, double>)
				return lgamma(data);
			else if constexpr (std::same_as<T, float>)
				return lgammaf(data);
			else
				return lgammal(data);
		}

		template <typename T>
		constexpr static double true_gamma(T data) noexcept {
			if constexpr (std::same_as<T, double>)
				return tgamma(data);
			else if constexpr (std::same_as<T, float>)
				return tgammaf(data);
			else
				return tgammal(data);
		}

		constexpr static double upper_incomplete_gamma_integer(uint32_t alpha, double rate) noexcept {
			if (alpha <= 0 || rate < 0) return 0.0; // Domain protection

			double sum = 0;
			for (uint32_t k = 0; k < alpha; ++k) {
				double log_term = k * Math::natural_logarithm(rate) - ln_abs_gamma(k + 1);
				sum += Math::exponential(log_term);
			}

			return Math::exponential(ln_abs_gamma(alpha) - rate) * sum;
		}

		constexpr static uint64_t choose(uint32_t n, uint32_t r) noexcept { //. pascal table next?
			if (r == 0 || r == n)
				return 1;
			if (r > (n >> 1))
				r = n - r;

			uint64_t result = 1;
			for (uint32_t i = 1; i <= r; ++i)
				result *= (n - i + 1) / i;

			return result;
		}

		template <DSA::concepts::Comparable T>
		constexpr static uint64_t permutation_identity_of(const T* const initial, uint8_t length, const T* const target)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
		{
			T available[18];
			Memory::copy_construct_range(available, available + 18, initial);
			uint8_t top = length;
			uint64_t identity = 0;

			for (uint8_t index = 0; index < length; ++index) {
				const auto& data = target[index];
				uint8_t position = 0;
				while (position < top && data != available[position])
					++position;
				identity += position * Constants::Factorials[length - index];
				while (++position < top)
					available[position - 1] = available[position];
				--top;
			}

			Memory::destroy_range(available, available + 18);

			return identity;
		}

		template <DSA::concepts::Comparable T>
		constexpr static void permute(const T* const data, uint8_t length, uint64_t identity)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
		{
			T available[18];
			Memory::copy_construct_range(available, available + 18, data);

			for (uint8_t index = 0; index < length; ++index) {
				const uint64_t factorial = Constants::Factorials[length - index];
				data[index] = available[identity / factorial + index];
				identity %= factorial;
			}

			Memory::destroy_range(available, available + 18);
		}

	}

}