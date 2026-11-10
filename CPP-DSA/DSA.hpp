#pragma once

#include "Memory.hpp"

namespace DSA {

	using namespace Algorithms;
	using namespace Algorithms::Memory;

	struct EMPTY_STRUCT {};

	namespace concepts {

		template <typename T, typename INDEX = size_t>
		concept HasCapacity = std::is_fundamental_v<INDEX> && requires(T t) {

			{ t.get_capacity() } -> std::convertible_to<INDEX>;

		};

		template <typename T, typename INDEX = size_t>
		concept HasMutableCapacity = HasCapacity<T, INDEX> && requires(T t, INDEX newCapacity) {

			{ t.set_capacity(newCapacity) };

		};

		template <typename T>
		concept HasData = requires(T t) {

			{ t.get_data() } -> std::convertible_to<typename T::T*>;

		};

		template <typename T>
		concept HasMutableData = HasData<T> && requires(T t, typename T::T * newData) {

			{ t.set_data(newData) };

		};

	}

}