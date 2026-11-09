#pragma once

#include <concepts>

//#include <random>

namespace Algorithms {

	template <typename T>
		requires std::is_fundamental_v<T>
	constexpr static T random() noexcept {
		return 0;
	}

	template <typename T>
		requires std::is_fundamental_v<T>
	constexpr static T random(T minimum, T maximum) noexcept {
		return 0;
	}

}