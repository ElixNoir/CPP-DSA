#pragma once

#include <concepts>

namespace DSA {

	namespace concepts {

		template <typename T, typename U = T>
		concept Addable = std::is_fundamental_v<T> || requires(T a, T b) {

			{ a + b } -> std::same_as<U>;
			{ b + a } -> std::same_as<U>;

		};

	}

}