#pragma once

#include "Combinatorics.hpp"
#include "Selection.hpp"

namespace Algorithms {

	namespace Constants {

		constexpr static double square_root_of_3 = 1.7320508075688772;

	}

	namespace Statistics {

#pragma region Exponential Distribution

		constexpr static double exponential_cdf(double data, double rate) noexcept {
			return 1 - Arithmetic::exponential(-rate * data);
		}

		constexpr static double exponential_expected_value(double rate) noexcept {
			return 1 / rate;
		}

		constexpr static double exponential_pdf(double data, double rate) noexcept {
			return rate * Arithmetic::exponential(-rate * data);
		}

		constexpr static double exponential_variance(double rate) noexcept {
			return 1 / (rate * rate);
		}

#pragma endregion

#pragma region Gamma Distribution

		constexpr static double gamma_cdf(double data, double alpha, double rate) noexcept {
			double sum = 0;
			for (int k = 0; k < alpha; ++k)
				sum += Arithmetic::power(rate * data, k) * Arithmetic::exponential(-rate * data) / Constants::Factorials[k];
			return sum;
		}

		constexpr static double gamma_expected_value(double alpha, double rate) noexcept {
			return alpha / (rate * rate);
		}

		constexpr static double gamma_pdf(double data, double alpha, double rate) noexcept {
			return Arithmetic::power(rate, alpha) * Arithmetic::power(data, alpha - 1) * Arithmetic::exponential(-rate * data) / Combinatorics::gamma(data);
		}

		constexpr static double gamma_variance(double alpha, double rate) noexcept {
			return alpha / rate;
		}

#pragma endregion

		// gamma-poisson

#pragma region Geometric Distribution

		constexpr static double geometric_cdf(double probability, double k) noexcept {
			return probability * Arithmetic::power(1 - probability, k - 1);
		}

		constexpr static double geometric_expected_value(double probability) noexcept {
			return 1 / probability;
		}

		// number of failures
		/*constexpr static double geometric_expected_value(double probability) noexcept {
			return (1 / probability) / probability;
		}*/

		constexpr static double geometric_pdf(double probability, double k) noexcept {
			return probability * Arithmetic::power(1 - probability, k - 1);
		}

		constexpr static double geometric_variance(double probability) noexcept {
			return (1 - probability) / (probability * probability);
		}

#pragma endregion

#pragma region Normal Distribution

		constexpr static double normal_cdf(double deviation, double standardDeviation) noexcept {
			return erfc(-deviation / (standardDeviation * Constants::square_root_of_2)) / 2;
		}

		constexpr static double normal_expected_value(double deviation, double variance) noexcept {
			return -Arithmetic::square(deviation) / (variance * 2);
		}

		// -inf < x < inf
		constexpr static double normal_pdf(double deviation, double variance) noexcept {
			return Arithmetic::exponential(normal_expected_value(deviation, variance)) / standard_deviation(variance * Constants::pies);
		}

		constexpr static double normal_variance(double variance) noexcept {
			return variance;
		}

#pragma endregion

#pragma region Poisson Distribution

		constexpr static double poisson_cdf(double k, double rate) noexcept {
			double sum = 0;
			for (int i = 0; i < k; ++i)
				sum += Arithmetic::power(rate, i) * Arithmetic::exponential(-rate) / Constants::Factorials[i];
			return sum;
		}

		constexpr static double poisson_expected_value(double rate) noexcept {
			return rate;
		}

		// -inf < x < inf
		constexpr static double poisson_pdf(double k, double rate) noexcept {
			return Arithmetic::power(rate, k) * Arithmetic::exponential(-rate) / Combinatorics::gamma(k);
		}

		constexpr static double poisson_variance(double rate) noexcept {
			return rate;
		}

#pragma endregion

#pragma region Uniform Distribution

		constexpr static double uniform_cdf(double data, double minimumX, double maximumX) noexcept {
			return (data - minimumX) / (maximumX - minimumX);
		}

		constexpr double uniform_cdf_piecewise(double x, double minimumX, double maximumX) noexcept {
			if (x < minimumX)
				return 0;
			if (x > maximumX)
				return 1;
			return uniform_cdf(minimumX, maximumX, x);
		}

		constexpr double uniform_cdf_deviation(double deviation, double sigma) noexcept {
			return (deviation / (sigma * Constants::square_root_of_3) + 1) / 2;
		}

		constexpr double uniform_cdf_deviation_piecewise(double deviation, double sigma) noexcept {
			const double r = sigma * Constants::square_root_of_3;
			if (abs(deviation) > r)
				return deviation > 0;
			return (deviation / r + 1) / 2;
		}

		constexpr static double uniform_expected_value(double minimumX, double maximumX) noexcept {
			return (maximumX + minimumX) / 2;
		}

		constexpr static double uniform_pdf(double minimumX, double maximumX) noexcept {
			return 1 / (maximumX - minimumX);
		}

		constexpr static double uniform_variance(double minimumX, double maximumX) noexcept {
			return Arithmetic::square(maximumX - minimumX) / 12;
		}

#pragma endregion

#pragma region General

		template <typename T>
		constexpr static T deviation(double data, double expectedValue) noexcept {
			return data - expectedValue;
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static T mean_of(ITERATOR begin, ITERATOR end) noexcept {
			T count = 0;
			T sum = 0;
			while (begin != end) {
				sum += *(begin++);
				++count;
			}
			return sum / count;
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static T mean_of(ITERATOR begin, ITERATOR end, size_t count) noexcept {
			T sum = Arithmetic::sum(begin, end);
			return sum / count;
		}

		template <DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static T median_of(T* data, size_t count)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			return Selection::quick_select(data, 0, count, (count + 1) >> 1);
		}

		constexpr static double standardize(double deviation, double standardDeviation) noexcept {
			return z_score(deviation * Constants::square_root_of_2, standardDeviation);
		}

		constexpr static double standard_deviation(double variance) noexcept {
			return Arithmetic::square_root(variance);
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static T variance_of(ITERATOR begin, ITERATOR end, size_t count) noexcept {
			T mean = mean_of(begin, end, count);
			T squareDifference = 0;
			while (begin != end)
				squareDifference += Arithmetic::square(*(begin++) - mean);
			return squareDifference / count;
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static T welford_variance_of(ITERATOR begin, ITERATOR end) noexcept {
			T mean = 0;
			T mean2 = 0;
			T count = 0;

			while (begin != end) {
				++count;
				const T data = *(begin++);
				const T delta = data - mean;
				mean += delta / count;
				mean2 += delta * (data - mean);
			}

			return mean2 / count;
		}

		constexpr static double z_score(double deviation, double standardDeviation) noexcept {
			return deviation / standardDeviation;
		}

#pragma endregion

		constexpr static bool is_markov_chain() noexcept {
			return false;
		}

		constexpr static double probability_of_switch_to_a(double probabilityOfA, double probASwitch, double probBSwitch) noexcept {
			return probabilityOfA * (1 - probASwitch) + (1 - probabilityOfA) * probBSwitch;
		}

	}

}