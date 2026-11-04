#pragma once

#include <concepts>

namespace DSA {

	namespace concepts {
		
		template <typename T>
		concept LinkedListNode = requires(T t) {

			typename T::INDEX;
			typename T::T;

			{ t.next } -> std::convertible_to<typename T::INDEX>;
			{ t.value } -> std::same_as<typename T::T>;

		};

	}

	template <typename _T, std::unsigned_integral _INDEX = size_t>
	struct LinkedListNode {

		using INDEX = _INDEX;
		using T = _T;

		INDEX next;
		T value;

	};

}