#pragma once

#include "Comparable.hpp"

namespace Algorithms {

	namespace Aggregation {

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

	}

}