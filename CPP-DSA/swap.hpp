#pragma once

#include <concepts>

namespace Algorithms {

	template <typename T>
	constexpr static void swap(T& a, T& b)
		noexcept(
			std::is_nothrow_move_constructible_v<T>
			&& std::is_nothrow_move_assignable_v<T>)
	{
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template <typename T>
		requires std::is_fundamental_v<T>
	constexpr static void swap_if(T& a, T& b, bool condition) { // branchless
		T d = (a ^ b) & -static_cast<T>(condition);
		a ^= d;
		b ^= d;
	}

	template <typename T>
	constexpr static void swap_if(T& a, T& b, bool condition) {
		if (condition)
			swap(a, b);
	}

	template <auto CONDITION_FUNCTION, typename T>
		requires std::is_invocable_r_v<bool, decltype(CONDITION_FUNCTION), const T&, const T&>
	constexpr static void swap_if(T& a, T& b) {
		swap_if(a, b, CONDITION_FUNCTION(a, b));
	}

}