#pragma once

#include "Addable.hpp"
#include "Constants.hpp"
#include "Iterator.hpp"
#include "Multipliable.hpp"
#include "unroll.hpp"

#include <limits>

namespace Algorithms {

	namespace Arithmetic {

#pragma region Bases & Powers

		[[nodiscard]] constexpr static double exponential(double exponent) noexcept {
			if consteval {
				double sum = 1.0;
				double term = 1.0;
				for (int i = 1; i < 20; ++i) {
					term *= exponent / i;
					sum += term;
				}
				return sum;
			}
			else {
				return exp(exponent);
			}
		}

		[[nodiscard]] consteval static int64_t ipower(int64_t base, int exponent, int64_t result = 1) noexcept {
			return exponent < 1 ? result : ipower(base * base, exponent >> 1, (exponent & 1) ? result * base : result);
		}

		[[nodiscard]] constexpr double static power(double base, double exponent) noexcept {
			if consteval {
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
			else {
				return pow(base, exponent);
			}
		}

		template <typename T>
			requires DSA::concepts::Multipliable<T>
		[[nodiscard]] constexpr static T square(T data) noexcept {
			return data * data;
		}

		[[nodiscard]] constexpr static double square_root(double data) noexcept {
			if consteval {
				if (data < 0)
					return std::numeric_limits<double>::quiet_NaN();
				if (data == 0 || data == 1)
					return data;

				double low = 0;
				double high = (data < 1) ? 1 : data;
				double mid = 0;

				while (true) {
					mid = (low + high) / 2;
					double midSquared = mid * mid;

					if (midSquared > data)
						high = mid;
					else
						low = mid;

					if ((high - low) < 1e-10)
						break;
				}
				return mid;
			}
			else {
				return sqrt(data);
			}
		}

#pragma endregion

		template <std::unsigned_integral T, bool SAFE = false>
		[[nodiscard]] constexpr static bool is_multiple_of(T value, T base)
			noexcept (SAFE)
		{
			if constexpr (SAFE)
				if (base == 0)
					return false;
			return value % base == 0;
		}

#pragma region Ranges

		/*template <std::unsigned_integral T>
		[[nodiscard]] constexpr static T product(T minimum, T maximum) noexcept {
			return Constants::Factorials[maximum] / Constants::Factorials[minimum - 1];
		}*/

		template <typename ITERATOR, typename T = std::iter_value_t<ITERATOR>>
			requires DSA::concepts::InputIteratorOf<ITERATOR, T>&& DSA::concepts::Multipliable<T>
		[[nodiscard]] constexpr static T product(ITERATOR begin, ITERATOR end) noexcept {
			T product = 1;
			while (begin != end)
				product *= *(begin++);
			return product;
		}

		template <typename ITERATOR, typename T = std::iter_value_t<ITERATOR>>
			requires DSA::concepts::InputIteratorOf<ITERATOR, T>&& DSA::concepts::Multipliable<T>
		[[nodiscard]] constexpr static T product4(ITERATOR begin, ITERATOR end) noexcept {
			T product = 1;
			unroll_forward_iterator<4>(begin, end, [&](auto) {
				product *= *(begin++);
			});
			return product;
		}

		template <std::unsigned_integral T>
		[[nodiscard]] constexpr static T sum(T minimum, T maximum) noexcept {
			return (maximum - minimum + 1) * (maximum + minimum) >> 1;
		}

		template <typename ITERATOR, typename T = std::iter_value_t<ITERATOR>>
			requires DSA::concepts::InputIteratorOf<ITERATOR, T>&& DSA::concepts::Addable<T>
		[[nodiscard]] constexpr static T sum(ITERATOR begin, ITERATOR end) noexcept {
			T sum = 0;
			while (begin != end)
				sum += *(begin++);
			return sum;
		}

		template <typename ITERATOR, typename T = std::iter_value_t<ITERATOR>>
			requires DSA::concepts::InputIteratorOf<ITERATOR, T>&& DSA::concepts::Addable<T>
		[[nodiscard]] constexpr static T sum4(ITERATOR begin, ITERATOR end) noexcept {
			T sum = 0;
			unroll_forward_iterator<4>(begin, end, [&](auto) {
				sum += *(begin++);
			});
			return sum;
		}

#pragma endregion

	}

}