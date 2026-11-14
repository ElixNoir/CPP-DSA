#pragma once

#include "hash.hpp"

namespace DSA {

	template <typename T>
	struct View {

		T* data;

		constexpr auto hash() const noexcept {
			return Algorithms::hash(data);
		}

	};

}