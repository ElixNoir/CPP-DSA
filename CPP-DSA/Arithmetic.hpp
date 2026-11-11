#pragma once

#include "Addable.hpp"
#include "Constants.hpp"
#include "Multipliable.hpp"
#include "Traits.hpp"

#include <cmath>
#include <limits>

namespace Algorithms {

	namespace Arithmetic {

#pragma region Bases & Powers

#pragma region e

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static double exponential(T exponent) noexcept {
			constexpr auto t = [](auto x) { return std::log(x); };
			if constexpr (!is_const_invocable<decltype(t), T>)
				return std::exp(exponent);
			else if consteval {
				double sum = 1.0;
				double term = 1.0;
				for (int i = 1; i < 30; ++i) {
					term *= exponent / i;
					sum += term;
				}

				return sum;
			}
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static double natural_logarithm(T value) {
			constexpr auto t = [](auto x) { return std::log(x); };
			if constexpr (!is_const_invocable<decltype(t), T>)
				return std::log(value);
			else if consteval {
				if (value < 0.0 || value != value)
					return std::numeric_limits<double>::quiet_NaN();
				if (value == 0.0)
					return -std::numeric_limits<double>::infinity();
				if (value == std::numeric_limits<double>::infinity())
					return value;
				if (value == 1.0)
					return 0.0;

				int k = 0;
				double m = value;

				while (m > Constants::square_root_of_2) {
					if (m > 1073741824.0) {
						m *= 9.31322574615478515625e-10;
						k += 30;
					}
					else {
						m /= 2;
						++k;
					}
				}
				while (m < Constants::inverted_square_root_of_2) {
					if (m < 9.31322574615478515625e-10) {
						m *= 1073741824.0;
						k -= 30;
					}
					else {
						m *= 2.0;
						--k;
					}
				}

				const double y = (m - 1.0) / (m + 1.0);
				const double y2 = y * y;

				double term = y;
				double sum = y;

				for (int i = 3; i <= 25; i += 2) {
					term *= y2;
					const double next_sum = sum + (term / static_cast<double>(i));
					if (next_sum == sum)
						break;
					sum = next_sum;
				}

				return (sum * 2) + (static_cast<double>(k) * Constants::natural_logarithm_of_2);
			}
		}

#pragma endregion

		[[nodiscard]] consteval static int64_t ipower(int64_t base, int exponent, int64_t result = 1) noexcept {
			return exponent < 1 ? result : ipower(base * base, exponent >> 1, (exponent & 1) ? result * base : result);
		}

		template <typename T, typename U>
			requires (std::is_fundamental_v<T> && std::is_fundamental_v<U>)
		[[nodiscard]] constexpr static auto power(T base, U exponent) noexcept {
			constexpr auto t = [](auto x, auto y) { return std::pow(x, y); };
			if constexpr (!is_const_invocable<decltype(t), T>)
				return std::pow(base, exponent);
			else if consteval {
				if constexpr (std::unsigned_integral<T> && std::unsigned_integral<U>)
					return ipower(base, exponent);
				else {
					if (exponent == 0)
						return 1.0;

					double result = 1.0;
					double p = (exponent > 0) ? base : 1.0 / base;
					uint32_t abs_exp = static_cast<uint32_t>((exponent > 0) ? exponent : -exponent);
					while (abs_exp > 0) {
						if (abs_exp & 1)
							result *= p;
						p *= p;
						abs_exp /= 2;
					}

					return result;
				}
			}
		}

		template <typename T>
			requires DSA::concepts::Multipliable<T>
		[[nodiscard]] constexpr static T cube(T value) noexcept {
			return value * value * value;
		}

		template <typename T>
			requires DSA::concepts::Multipliable<T>
		[[nodiscard]] constexpr static T square(T value) noexcept {
			return value * value;
		}

#pragma region Roots

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static auto cube_root(T value) noexcept {
			constexpr auto t = [](auto x) { return std::cbrt(x); };

			if constexpr (!is_const_invocable<decltype(t), T>)
				return std::cbrt(value);
			else if consteval {
				if (value == 0 || value == 1 || value == -1)
					return value;

				const bool negative = value < 0;
				const double target = negative ? -value : value;
				double x = (target >= 1) ? target : 1;
				while (true) {
					const double next = (x * 2 + target / (x * x)) / 3;
					if (next == x)
						break;
					x = next;
				}

				return negative ? -x : x;
			}
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static auto square_root(T value) noexcept {
			constexpr auto t = [](auto x) { return std::sqrt(x); };
			if constexpr (!is_const_invocable<decltype(t), T>)
				return std::sqrt(value);
			else if consteval {
				if (value < 0)
					return std::numeric_limits<double>::quiet_NaN();
				if (value == 0 || value == 1)
					return value;

				double low = 0;
				double high = (value < 1) ? 1 : value;
				double mid = 0;
				while (true) {
					mid = (low + high) / 2;
					double midSquared = mid * mid;

					if (midSquared > value)
						high = mid;
					else
						low = mid;

					if ((high - low) < 1e-10)
						break;
				}

				return mid;
			}
		}

#pragma endregion

#pragma endregion

#pragma region Ranges

		template <std::unsigned_integral T>
		[[nodiscard]] constexpr static T product(T minimum, T maximum) noexcept {
			return Constants::Factorials[maximum] / Constants::Factorials[minimum - 1];
		}

		template <std::unsigned_integral T>
		[[nodiscard]] constexpr static T sum(T minimum, T maximum) noexcept {
			return (maximum - minimum + 1) * (maximum + minimum) >> 1;
		}

#pragma endregion

	}

}