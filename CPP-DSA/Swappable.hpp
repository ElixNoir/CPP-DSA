#pragma once

#include <concepts>

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Swappable = requires(T& t, T& other) {

			{ t.swap(other) };

		};

	}

}

namespace Algorithms {

	template <typename T>
		requires DSA::concepts::Swappable<T>
	constexpr static void swap(T& a, T& b)
		noexcept(noexcept(
			std::declval<T&>()
			.swap(std::declval<T&>())))
	{
		a.swap(b);
	}

}