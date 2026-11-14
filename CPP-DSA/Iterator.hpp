#pragma once

#include <iterator>

namespace DSA {

	namespace concepts {

		template<typename It, typename T>
		concept InputIterator = std::input_iterator<It> && std::same_as<std::iter_value_t<It>, T>;

	}

	template <typename T>
	struct is_copy_iterator : std::true_type {};

	template <typename T>
	struct is_copy_iterator<std::move_iterator<T>> : std::false_type {};

	template <typename T>
	constexpr static bool is_copy_iterator_v = is_copy_iterator<T>::value;

	template <typename T>
	struct is_move_iterator : std::false_type {};

	template <typename T>
	struct is_move_iterator<std::move_iterator<T>> : std::true_type {};

	template <typename T>
	constexpr static bool is_move_iterator_v = is_move_iterator<T>::value;

}