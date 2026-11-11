#pragma once

#include <iterator>

namespace DSA {

	namespace concepts {

		template<typename It, typename T>
		concept InputIterator = std::input_iterator<It> && std::same_as<std::iter_value_t<It>, T>;

	}

}