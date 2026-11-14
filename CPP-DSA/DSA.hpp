#pragma once

#include "Math.hpp"
#include "Memory.hpp"

namespace DSA {

	using namespace Algorithms;
	using namespace Algorithms::Math;
	using namespace Algorithms::Memory;

	struct EMPTY_STRUCT {};

	namespace concepts {

#pragma region Capacity

		template <typename T, typename INDEX = size_t>
		concept HasCapacity = std::is_fundamental_v<INDEX> && requires(T t) {

			{ t.get_capacity() } -> std::convertible_to<INDEX>;

		};

		template <typename T, typename INDEX = size_t>
		concept HasMutableCapacity = HasCapacity<T, INDEX> && requires(T t, INDEX newCapacity) {

			{ t.set_capacity(newCapacity) };

		};

#pragma endregion

#pragma region Data

		template <typename T>
		concept HasData = requires(T t) {

			{ t.get_data() } -> std::convertible_to<typename T::T*>;

		};

		template <typename T>
		concept HasMutableData = HasData<T> && requires(T t, typename T::T * newData) {

			{ t.set_data(newData) };

		};

#pragma endregion

#pragma region Size

		template <typename T>
		concept HasSize = requires(T t) {

			{ t.get_size() } -> std::same_as<typename T::INDEX>;

		};

		template <typename T>
		concept HasMutableSize = HasData<T> && requires(T t, typename T::INDEX newSize) {

			{ t.set_size(newSize) };

		};

#pragma endregion

	}

}