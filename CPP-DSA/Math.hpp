#pragma once

namespace Algorithms {

	namespace Math {

		template <typename T>
		constexpr static T& maximum(T& a, T& b)
			noexcept(noexcept(a > b))
		{
			return a > b ? a : b;
		}

		template <typename T>
		constexpr static const T& maximum(const T& a, const T& b)
			noexcept(noexcept(a > b))
		{
			return a > b ? a : b;
		}

		template <typename T>
		constexpr static T& minimum(T& a, T& b)
			noexcept(noexcept(a < b))
		{
			return a < b ? a : b;
		}

		template <typename T>
		constexpr static const T& minimum(const T& a, const T& b)
			noexcept(noexcept(a < b))
		{
			return a < b ? a : b;
		}

	}

}