#pragma once

#include "Combinatorics.hpp"
#include "Constants.hpp"
#include "Selection.hpp"

namespace Algorithms {

	namespace Statistics {

#pragma region General

		template <typename T>
		constexpr static auto deviation(T value, T expectedValue) noexcept {
			return value - expectedValue;
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
			T sum = Math::sum(begin, end);
			return sum / count;
		}

		template <DSA::concepts::Comparable T>
			requires std::is_swappable_v<T>
		constexpr static T median_of(T* value, size_t count)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			return Selection::quick_select(value, 0, count, (count + 1) >> 1);
		}

		constexpr static double z_score(double deviation, double standardDeviation) noexcept {
			return deviation / standardDeviation;
		}

		constexpr static double standardize(double deviation, double standardDeviation) noexcept {
			return z_score(deviation * Constants::square_root_of_2, standardDeviation);
		}

		constexpr static double standard_deviation(double variance) noexcept {
			return Math::square_root(variance);
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static T variance_of(ITERATOR begin, ITERATOR end, size_t count) noexcept {
			T mean = mean_of(begin, end, count);
			T squareDifference = 0;
			while (begin != end)
				squareDifference += Math::square(*(begin++) - mean);
			return squareDifference / count;
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static T welford_variance_of(ITERATOR begin, ITERATOR end) noexcept {
			T mean = 0;
			T mean2 = 0;
			T count = 0;

			while (begin != end) {
				++count;
				const T value = *(begin++);
				const T delta = value - mean;
				mean += delta / count;
				mean2 += delta * (value - mean);
			}

			return mean2 / count;
		}

#pragma endregion

#pragma region Distribution

#pragma region Bernoulli Distribution

		constexpr static double bernoulli_expected_value(double probability) noexcept {
			return probability;
		}

		constexpr static double bernoulli_pmf(double probability, double value) noexcept {
			return Math::power(probability, value) * Math::power(1 - probability, 1 - value);
		}

		constexpr static double bernoulli_variance(double probability) noexcept {
			return probability * (1 - probability);
		}

		constexpr static double bernoulli_standard_deviation(double probability) noexcept {
			return Math::square_root(bernoulli_variance(probability));
		}

#pragma endregion

#pragma region Binomial Distribution

		constexpr static double binomial_cdf(uint64_t count, double probability, double k) noexcept {
			double sum = 0.0;
			for (int i = 0; i < k; ++i)
				sum += Combinatorics::choose(count, i) * Math::power(probability, i) * Math::power(1 - probability, count - i);
			return sum;
		}

		constexpr static double binomial_expected_value(uint64_t count, double probability) noexcept {
			return count * probability;
		} 

		constexpr static double binomial_pmf(uint64_t count, double probability, double k) noexcept {
			return Combinatorics::choose(count, k) * Math::power(probability, k) * Math::power(1 - probability, count - k);
		}

		constexpr static double binomial_variance(uint64_t count, double probability) noexcept {
			return count * probability * (1 - probability);
		}

		constexpr static double binomial_standard_deviation(uint64_t count, double probability) noexcept {
			return Math::square_root(binomial_variance(count, probability));
		}

#pragma endregion

#pragma region Exponential Distribution

#pragma region CDF

		constexpr static double exponential_greater_cdf(double rate, double value) noexcept {
			return Math::exponential(-value * rate);
		}

		constexpr static double exponential_less_cdf(double rate, double value) noexcept {
			return 1 - exponential_greater_cdf(value, rate);
		}

#pragma endregion

		constexpr static double exponential_expected_value(double rate) noexcept {
			return 1 / rate;
		}

		constexpr static double exponential_pdf(double rate, double value) noexcept {
			return rate * Math::exponential(-value * rate);
		}

		constexpr static double exponential_variance(double rate) noexcept {
			return 1 / (rate * rate);
		}

#pragma endregion

#pragma region Gamma Distribution

#pragma region CDF

		constexpr static double gamma_greater_cdf(uint64_t shape, double rate, double value) noexcept {
			double sum = 0;
			const double x = value * rate;
			for (int k = 0; k < shape; ++k)
				sum += Math::power(x, k) / Constants::Factorials[k];
			return sum * Math::exponential(-x);
		}

		constexpr static double gamma_shape_of_2_greater_cdf(double rate, double value) noexcept {
			const double d = value / rate;
			return Math::exponential(-d) * (d + 1);
		}

		constexpr static double gamma_less_cdf(uint64_t shape, double rate, double value) noexcept {
			return 1 - gamma_greater_cdf(value, shape, rate);
		}

		constexpr static double gamma_shape_of_2_less_cdf(double rate, double value) noexcept {
			return 1 - gamma_shape_of_2_greater_cdf(value, rate);
		}

#pragma endregion

		constexpr static double gamma_expected_value(double shape, double rate) noexcept {
			return shape / rate;
		}

		constexpr static double gamma_pdf(double shape, double rate, double value) noexcept {
			return Math::power(rate, shape) * Math::power(value, shape - 1) * Math::exponential(-value * rate) / Combinatorics::ln_abs_gamma(value);
		}

		constexpr static double gamma_variance(double shape, double rate) noexcept {
			return shape / (rate * rate);
		}

#pragma endregion

#pragma region Geometric Distribution

#pragma region CDF

		constexpr static double geometric_less_cdf(double probability, double k) noexcept {
			return Math::power(1 - probability, k);
		}

		constexpr static double geometric_greater_cdf(double probability, double k) noexcept {
			return 1 - geometric_less_cdf(probability, k);
		}

#pragma endregion

		constexpr static double geometric_expected_value(double probability) noexcept {
			return 1 / probability;
		}

		constexpr static double geometric_pdf(double probability, double k) noexcept {
			return probability * Math::power(1 - probability, k - 1);
		}

		constexpr static double geometric_variance(double probability) noexcept {
			return (1 - probability) / (probability * probability);
		}

#pragma endregion

#pragma region Normal Distribution

#pragma region CDF

		constexpr static double normal_less_cdf(double deviation, double standardDeviation) noexcept {
			return erfc(-deviation / (standardDeviation * Constants::square_root_of_2)) / 2;
		}

		constexpr static double normal_greater_cdf(double deviation, double standardDeviation) noexcept {
			return 1 - normal_less_cdf(deviation, standardDeviation);
		}

#pragma endregion

		constexpr static double normal_expected_value(double deviation, double variance) noexcept {
			return -Math::square(deviation) / (variance * 2);
		}

		constexpr static double normal_pdf(double deviation, double variance) noexcept {
			return Math::exponential(normal_expected_value(deviation, variance)) / standard_deviation(variance * Constants::pies);
		}

		constexpr static double normal_variance(double variance) noexcept {
			return variance;
		}

#pragma endregion

#pragma region Poisson Distribution

		// memoryless

#pragma region CDF

		constexpr static double poisson_greater_cdf(double rate, uint64_t k) noexcept {
			double sum = 0;
			for (int i = 0; i < k; ++i)
				sum += Math::power(rate, i) * Math::exponential(-rate) / Constants::Factorials[i];
			return sum;
		}

		constexpr static double poisson_less_cdf(double rate, uint64_t k) noexcept {
			return 1 - poisson_greater_cdf(rate, k);
		}

#pragma endregion

		constexpr static double poisson_cf(double rate) noexcept {
			return 0.0; //. TO-DO
		}

		constexpr static double poisson_entropy(double rate) noexcept {
			return 0.0; //. TO-DO
		}

		constexpr static double poisson_excess_kurtosis(double rate) noexcept {
			return 1 / rate;
		}

		constexpr static double poisson_expected_value(double rate, double time) noexcept {
			return rate * time;
		}

		constexpr static double poisson_fisher_information(double rate) noexcept {
			return 1 / rate;
		}

		constexpr static double poisson_mgf(double rate) noexcept {
			return 0.0; //. TO-DO
		}

		constexpr static double poisson_mean(double rate) noexcept {
			return rate;
		}

		constexpr static double poisson_median_approximate(double rate) noexcept {
			return rate + 1.0 / 3.0 - 1.0 / (rate * 50.0);
		}

		constexpr static double poisson_mode(double rate) noexcept {
			return 0.0; //. TO-DO
		}

		// -inf < x < inf
		constexpr static double poisson_pdf(double rate, uint64_t k) noexcept {
			return Math::power(rate, k) * Math::exponential(-rate) / Constants::Factorials[k];
		}

		constexpr static double poisson_pgf(double rate) noexcept {
			return 0.0; //. TO-DO
		}

		constexpr static double poisson_skewness(double rate) noexcept {
			return 1.0 / Math::square_root(rate);
		}

		constexpr static double poisson_variance(double rate) noexcept {
			return rate;
		}

#pragma endregion

#pragma region Uniform Distribution

#pragma region CDF

		constexpr static double uniform_cdf(double minimumX, double maximumX, double value) noexcept {
			return (value - minimumX) / (maximumX - minimumX);
		}

		constexpr double uniform_cdf_piecewise(double minimumX, double maximumX, double value) noexcept {
			if (value < minimumX)
				return 0;
			if (value > maximumX)
				return 1;
			return uniform_cdf(minimumX, maximumX, value);
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

#pragma endregion

		constexpr static double uniform_expected_value(double minimumX, double maximumX) noexcept {
			return (maximumX + minimumX) / 2;
		}

		constexpr static double uniform_pdf(double minimumX, double maximumX) noexcept {
			return 1 / (maximumX - minimumX);
		}

		constexpr static double uniform_variance(double minimumX, double maximumX) noexcept {
			return Math::square(maximumX - minimumX) / 12;
		}

#pragma endregion

#pragma endregion

		constexpr static bool is_markov_chain() noexcept {
			return false;
		}

		constexpr static double probability_of_switch_to_a(double probabilityOfA, double probASwitch, double probBSwitch) noexcept {
			return probabilityOfA * (1 - probASwitch) + (1 - probabilityOfA) * probBSwitch;
		}

	}

}