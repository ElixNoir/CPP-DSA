#pragma once

#include "Aggregation.hpp"
#include "Combinatorics.hpp"
#include "MathConstants.hpp"
#include "Selection.hpp"

namespace Algorithms::Statistics {

#pragma region General

	template <typename T>
	constexpr static auto deviation(const T value, const T expectedValue) noexcept {
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
	constexpr static T mean_of(ITERATOR begin, ITERATOR end, const size_t count) noexcept {
		T sum = Aggregation::sum(begin, end);
		return sum / count;
	}

	template <DSA::concepts::Comparable T>
		requires std::is_swappable_v<T>
	constexpr static T median_of(T* value, const size_t count)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		return Selection::quick_select(value, 0, count, (count + 1) >> 1);
	}

	constexpr static double z_score(const double deviation, const double standardDeviation) noexcept {
		return deviation / standardDeviation;
	}

	constexpr static double standardize(const double deviation, const double standardDeviation) noexcept {
		return z_score(deviation * Constants::square_root_of_2, standardDeviation);
	}

	constexpr static double standard_deviation(const double variance) noexcept {
		return Math::square_root(variance);
	}

	template <typename T, std::input_iterator ITERATOR>
	constexpr static T variance_of(ITERATOR begin, ITERATOR end, const size_t count) noexcept {
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

	constexpr static double bernoulli_expected_value(const double probability) noexcept {
		return probability;
	}

	constexpr static double bernoulli_pmf(const double probability, const double value) noexcept {
		return Math::power(probability, value)
		       * Math::power(1 - probability, 1 - value);
	}

	constexpr static double bernoulli_variance(const double probability) noexcept {
		return probability * (1 - probability);
	}

	constexpr static double bernoulli_standard_deviation(const double probability) noexcept {
		return Math::square_root(bernoulli_variance(probability));
	}

#pragma endregion

#pragma region Binomial Distribution

	constexpr static double binomial_cdf(const uint64_t count, const double probability, const double k) noexcept {
		double sum = 0.0;
		for (int i = 0; i < k; ++i)
			sum += static_cast<double>(Combinatorics::choose(count, i)) * Math::power(probability, i) * Math::power(1 - probability, count - i);
		return sum;
	}

	constexpr static double binomial_expected_value(const uint64_t count, const double probability) noexcept {
		return static_cast<double>(count) * probability;
	}

	constexpr static double binomial_pmf(const uint64_t count, const double probability, const double k) noexcept {
		return static_cast<double>(
			       Combinatorics::choose(count, static_cast<uint32_t>(k)))
		       * Math::power(probability, k)
		       * Math::power(1 - probability, static_cast<double>(count) - k);
	}

	constexpr static double binomial_variance(const uint64_t count, const double probability) noexcept {
		return static_cast<double>(count) * probability * (1 - probability);
	}

	constexpr static double binomial_standard_deviation(const uint64_t count, const double probability) noexcept {
		return Math::square_root(binomial_variance(count, probability));
	}

#pragma endregion

#pragma region Exponential Distribution

#pragma region CDF

	constexpr static double exponential_greater_cdf(const double rate, const double value) noexcept {
		return Math::exponential(-value * rate);
	}

	constexpr static double exponential_less_cdf(const double rate, const double value) noexcept {
		return 1.0 - exponential_greater_cdf(rate, value);
	}

#pragma endregion

	constexpr static double exponential_expected_value(const double rate) noexcept {
		return 1.0 / rate;
	}

	constexpr static double exponential_pdf(const double rate, const double value) noexcept {
		return rate * Math::exponential(-value * rate);
	}

	constexpr static double exponential_variance(const double rate) noexcept {
		return 1.0 / (rate * rate);
	}

#pragma endregion

#pragma region Gamma Distribution

#pragma region CDF

	constexpr static double gamma_greater_cdf(const uint64_t shape, const double rate, const double value) noexcept {
		double sum = 0;
		const double x = value * rate;
		for (int k = 0; k < shape; ++k)
			sum += Math::power(x, k) / static_cast<double>(Constants::Factorials[k]);
		return sum * Math::exponential(-x);
	}

	constexpr static double gamma_shape_of_2_greater_cdf(const double rate, const double value) noexcept {
		const double d = value / rate;
		return Math::exponential(-d) * (d + 1.0);
	}

	constexpr static double gamma_less_cdf(const uint64_t shape, const double rate, const double value) noexcept {
		return 1.0 - gamma_greater_cdf(shape, rate, value);
	}

	constexpr static double gamma_shape_of_2_less_cdf(const double rate, const double value) noexcept {
		return 1.0 - gamma_shape_of_2_greater_cdf(value, rate);
	}

#pragma endregion

	constexpr static double gamma_expected_value(const double shape, const double rate) noexcept {
		return shape / rate;
	}

	constexpr static double gamma_pdf(const double shape, const double rate, const double value) noexcept {
		return Math::power(rate, shape)
		       * Math::power(value, shape - 1)
		       * Math::exponential(-value * rate)
		       / Combinatorics::ln_abs_gamma(value);
	}

	constexpr static double gamma_variance(const double shape, const double rate) noexcept {
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

	constexpr static double geometric_expected_value(const double probability) noexcept {
		return 1.0 / probability;
	}

	constexpr static double geometric_pdf(const double probability, const double k) noexcept {
		return probability * Math::power(1.0 - probability, k - 1.0);
	}

	constexpr static double geometric_variance(const double probability) noexcept {
		return (1.0 - probability) / (probability * probability);
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

	constexpr static double normal_expected_value(const double deviation, const double variance) noexcept {
		return -Math::square(deviation) / (variance * 2.0);
	}

	constexpr static double normal_pdf(const double deviation, const double variance) noexcept {
		return Math::exponential(normal_expected_value(deviation, variance))
		       / standard_deviation(variance * Constants::pies);
	}

	constexpr static double normal_variance(const double variance) noexcept {
		return variance;
	}

#pragma endregion

#pragma region Poisson Distribution

	// memoryless

#pragma region CDF

	constexpr static double poisson_greater_cdf(const double rate, const uint64_t k) noexcept {
		double sum = 0;
		for (int i = 0; i < k; ++i)
			sum += Math::power(rate, i) * Math::exponential(-rate) / static_cast<double>(Constants::Factorials[i]);
		return sum;
	}

	constexpr static double poisson_less_cdf(const double rate, const uint64_t k) noexcept {
		return 1.0 - poisson_greater_cdf(rate, k);
	}

#pragma endregion

	constexpr static double poisson_cf(const double rate) noexcept {
		return 0.0; //. TO-DO
	}

	constexpr static double poisson_entropy(const double rate) noexcept {
		return 0.0; //. TO-DO
	}

	constexpr static double poisson_excess_kurtosis(const double rate) noexcept {
		return 1.0 / rate;
	}

	constexpr static double poisson_expected_value(const double rate, const double time) noexcept {
		return rate * time;
	}

	constexpr static double poisson_fisher_information(const double rate) noexcept {
		return 1.0 / rate;
	}

	constexpr static double poisson_mgf(const double rate) noexcept {
		return 0.0; //. TO-DO
	}

	constexpr static double poisson_mean(const double rate) noexcept {
		return rate;
	}

	constexpr static double poisson_median_approximate(const double rate) noexcept {
		return rate + 1.0 / 3.0 - 1.0 / (rate * 50.0);
	}

	constexpr static double poisson_mode(double rate) noexcept {
		return 0.0; //. TO-DO
	}

	// -inf < x < inf
	constexpr static double poisson_pdf(const double rate, const uint64_t k) noexcept {
		return Math::power(rate, k) * Math::exponential(-rate) / static_cast<double>(Constants::Factorials[k]);
	}

	constexpr static double poisson_pgf(const double rate) noexcept {
		return 0.0; //. TO-DO
	}

	constexpr static double poisson_skewness(const double rate) noexcept {
		return 1.0 / Math::square_root(rate);
	}

	constexpr static double poisson_variance(const double rate) noexcept {
		return rate;
	}

#pragma endregion

#pragma region Uniform Distribution

#pragma region CDF

	constexpr static double uniform_cdf(const double minimumX, const double maximumX, const double value) noexcept {
		return (value - minimumX) / (maximumX - minimumX);
	}

	constexpr double uniform_cdf_piecewise(const double minimumX, const double maximumX, const double value) noexcept {
		if (value < minimumX)
			return 0;
		if (value > maximumX)
			return 1.0;
		return uniform_cdf(minimumX, maximumX, value);
	}

	constexpr double uniform_cdf_deviation(const double deviation, const double sigma) noexcept {
		return (deviation / (sigma * Constants::square_root_of_3) + 1.0) / 2.0;
	}

	constexpr double uniform_cdf_deviation_piecewise(const double deviation, const double sigma) noexcept {
		const double r = sigma * Constants::square_root_of_3;
		if (Math::absolute_value(deviation) > r)
			return deviation > 0;
		return (deviation / r + 1) / 2;
	}

#pragma endregion

	constexpr static double uniform_expected_value(const double minimumX, const double maximumX) noexcept {
		return (maximumX + minimumX) / 2.0;
	}

	constexpr static double uniform_pdf(const double minimumX, const double maximumX) noexcept {
		return 1.0 / (maximumX - minimumX);
	}

	constexpr static double uniform_variance(const double minimumX, const double maximumX) noexcept {
		return Math::square(maximumX - minimumX) / 12.0;
	}

#pragma endregion

#pragma endregion

	constexpr static bool is_markov_chain() noexcept {
		return false;
	}

	constexpr static double probability_of_switch_to_a(const double probabilityOfA, const double probASwitch, const double probBSwitch) noexcept {
		return probabilityOfA * (1.0 - probASwitch) + (1.0 - probabilityOfA) * probBSwitch;
	}

}
