#pragma once

#include <concepts>

namespace DSA {

	namespace concepts {

		template <typename T>
		concept DoublyLinkedListNode = requires(T node) {

			typename T::INDEX;
			typename T::T;

			{ node.next } -> std::convertible_to<typename T::INDEX>;
			{ node.previous } -> std::convertible_to<typename T::INDEX>;
			{ node.value } -> std::same_as<typename T::T>;

		};

	}

	template <typename _T, std::unsigned_integral _INDEX = size_t>
	struct DoublyLinkedListNode {

		using INDEX = _INDEX;
		using T = _T;

		INDEX next;
		INDEX previous;
		T value;

	};

}