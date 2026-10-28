#pragma once

#include <concepts>

namespace DSA {
	
	namespace concepts {

		template <typename T>
		concept Hashable = requires(const T& t) {

			{ t.hash() } -> std::unsigned_integral;

		};

	}

}