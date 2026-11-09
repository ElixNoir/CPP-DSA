#pragma once

#include "Addable.hpp"
#include "Multipliable.hpp"

#include <iterator>
#include <limits>

namespace Algorithms {

	namespace Arithmetic {

        constexpr static double exponential(double exponent) noexcept {
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

        consteval static int64_t ipower(int64_t base, int exponent, int64_t result = 1) noexcept {
            return exponent < 1 ? result : ipower(base * base, exponent >> 1, (exponent & 1) ? result * base : result);
        }

        constexpr double static power(double base, double exponent) noexcept {
            if consteval {
                if (exponent == 0)
                    return 1.0;
                double result = 1.0;
                double p = (exponent > 0) ? base : 1.0 / base;
                unsigned int abs_exp = (exponent > 0) ? exponent : -exponent;
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
		constexpr static T square(T data) noexcept {
			return data * data;
		}

        constexpr static double square_root(double data) noexcept {
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

#pragma region Ranges

		template <typename T, std::input_iterator ITERATOR>
			requires DSA::concepts::Multipliable<T>
		constexpr static T product(ITERATOR begin, ITERATOR end) noexcept {
			return 0;
		}

        template <typename T, std::input_iterator ITERATOR>
            requires DSA::concepts::Addable<T>
        constexpr static T sum(ITERATOR begin, ITERATOR end) noexcept {
            return 0;
        }

#pragma endregion

	}

}