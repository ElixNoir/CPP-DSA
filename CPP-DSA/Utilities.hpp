#pragma once

namespace Utilities {

	template <typename T>
	constexpr T& maximum(const T& a, const T& b) noexcept {
		return a > b ? a : b;
	}

	template <typename T>
	constexpr T& minimum(const T& a, const T& b) noexcept {
		return a < b ? a : b;
	}

}