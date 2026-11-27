#pragma once

#include <concepts>

namespace DSA {

	namespace concepts {
		
		template <typename T>
		concept LinkedListNode = requires(T t) {

			typename T::INDEX;
			typename T::T;

			{ t.next } -> std::convertible_to<typename T::INDEX>;
			{ t.data } -> std::same_as<typename T::T>;

		};

	}

	template <typename T_, std::unsigned_integral INDEX_ = uint32_t>
	struct LinkedListNode {

		using INDEX = INDEX_;
		using T = T_;

		INDEX next;
		T data;

	};

}