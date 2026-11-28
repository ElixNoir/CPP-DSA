#pragma once

#include "../Structures/Addable.hpp"
#include "../Structures/Comparable.hpp"
#include "../Structures/Multipliable.hpp"
#include "../Structures/Iterator.hpp"
#include "unroll.hpp"

namespace Algorithms::Aggregation {

#pragma region Maximum & Minimum

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

#pragma endregion

	template <typename ITERATOR, typename T = std::iter_value_t<ITERATOR>>
		requires (DSA::concepts::InputIterator<ITERATOR, T> && DSA::concepts::Addable<T>)
	[[nodiscard]] constexpr static T sum(ITERATOR begin, ITERATOR end) noexcept {
		T sum = 0;
		while (begin != end)
			sum += *(begin++);

		return sum;
	}

	template <typename ITERATOR, typename T = std::iter_value_t<ITERATOR>>
		requires (DSA::concepts::InputIterator<ITERATOR, T> && DSA::concepts::Multipliable<T>)
	[[nodiscard]] constexpr static T product(ITERATOR begin, ITERATOR end) noexcept {
		T product = 1;
		while (begin != end)
			product *= *(begin++);

		return product;
	}

}
