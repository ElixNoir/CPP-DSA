#pragma once

#include "Comparable.hpp"

namespace Algorithms {

	enum Comparison : uint8_t {

		EQUAL,
		NOT_EQUAL,

		GREATER_THAN,
		GREATER_THAN_OR_EQUAL,

		LESS_THAN,
		LESS_THAN_OR_EQUAL

	};

	template <bool ASCENDING = true, DSA::concepts::Comparable T>
	constexpr static bool compare(const T& a, const T& b)
		noexcept (noexcept(ASCENDING ? a < b : a > b))
	{
		if constexpr (ASCENDING)
			return a < b;
		else
			return a > b;
	}

	template <Comparison COMPARISON, DSA::concepts::Comparable T>
	constexpr static bool compare(const T& a, const T& b) {
		if constexpr (COMPARISON == Comparison::EQUAL)
			return a == b;
		else if constexpr (COMPARISON == Comparison::NOT_EQUAL)
			return a != b;
		else if constexpr (COMPARISON == Comparison::GREATER_THAN)
			return a > b;
		else if constexpr (COMPARISON == Comparison::GREATER_THAN_OR_EQUAL)
			return a >= b;
		else if constexpr (COMPARISON == Comparison::LESS_THAN)
			return a < b;
		else
			return a <= b;
	}

}