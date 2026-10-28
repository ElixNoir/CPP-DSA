#pragma once

#include <concepts>

template <typename T>
	requires std::is_fundamental_v<T>
struct Primitive {

	T value;

	constexpr operator T() const noexcept {
		return value;
	}

	constexpr T hash() const noexcept {
		return value;
	}

	constexpr void swap(Primitive<T>& other) noexcept {
		T temporary = other.value;
		other.value = value;
		value = temporary;
	}

};