#pragma once

#include <concepts>

namespace DSA {

	namespace concepts {

		template <typename T, typename U = T>
		concept Comparable = std::is_fundamental_v<T> || requires(const T & t, const U & u) {

			{ t == u } -> std::same_as<bool>;
			{ t != u } -> std::same_as<bool>;
			{ t < u } -> std::same_as<bool>;
			{ t <= u } -> std::same_as<bool>;
			{ t > u } -> std::same_as<bool>;
			{ t >= u } -> std::same_as<bool>;

		};

	}

	namespace traits {

		template <typename T>
		struct is_expensively_comparable {
			static constexpr bool value = requires {
				{ T::is_expensively_comparable } -> std::same_as<const bool&>;
			} ? T::is_expensively_comparable : false;
		};

		template <typename T>
			requires std::is_fundamental_v<T>
		struct is_expensively_comparable<T> {
			static constexpr bool value = false;
		};

		template <typename T>
		constexpr bool is_expensively_comparable_v = is_expensively_comparable<T>::value;

	}

}