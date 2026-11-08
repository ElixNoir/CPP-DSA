#pragma once

#include <concepts>

namespace Algorithms {

	template <typename T>
		requires std::is_swappable_v<T>
	constexpr static void swap(T& a, T& b)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template <typename T>
		requires std::is_swappable_v<T>
	constexpr static void swap_many(T* a, T* b, size_t length)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		for (size_t i = 0; i < length; ++i)
			swap(a[i], b[i]);
	}

	template <typename T>
		requires std::is_swappable_v<T>
	constexpr static void swap_many(T* begin, const T* const end, T* other)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		while (begin != end)
			swap(*(begin++), *(other++));
	}

	template <typename T>
	constexpr static void swap_if(T& a, T& b, bool condition) noexcept
		requires std::is_fundamental_v<T>
	{ // branchless
		T d = (a ^ b) & -static_cast<T>(condition);
		a ^= d;
		b ^= d;
	}

	template <typename T>
		requires std::is_swappable_v<T>
	constexpr static void swap_if(T& a, T& b, bool condition)
		noexcept (std::is_nothrow_swappable_v<T>)
		requires (!std::is_fundamental_v<T>)
	{
		if (condition)
			swap(a, b);
	}

	template <auto CONDITION_FUNCTION, typename T>
		requires std::is_invocable_r_v<bool, decltype(CONDITION_FUNCTION), const T&, const T&>
	constexpr static void swap_if(T& a, T& b)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		swap_if(a, b, CONDITION_FUNCTION(a, b));
	}

	template <auto CONDITION_FUNCTION, typename T>
		requires std::is_invocable_r_v<bool, decltype(CONDITION_FUNCTION), const T&, const T&>
	constexpr static void swap_if_many(T* a, T* b, size_t length)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		for (size_t i = 0; i < length; ++i)
			swap_if<CONDITION_FUNCTION>(a[i], b[i]);
	}

	template <auto CONDITION_FUNCTION, typename T>
		requires std::is_invocable_r_v<bool, decltype(CONDITION_FUNCTION), const T&, const T&>
	constexpr static void swap_if_many(T* begin, const T* const end, T* other)
		noexcept (std::is_nothrow_swappable_v<T>)
	{
		while (begin != end)
			swap_if<CONDITION_FUNCTION>(*(begin++), *(other++));
	}

}
