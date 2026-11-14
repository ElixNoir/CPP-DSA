#pragma once

#include "Constants.hpp"

#include <concepts>

namespace DSA {

	template <typename T>
		requires std::is_fundamental_v<T>
	struct Vector2 {

		using Self = Vector2<T>;

		T x;
		T y;

#pragma region Methods

		constexpr T cross_product(Self other) const noexcept {
			return x * other.y - y * other.x;
		}

		constexpr T dot_product(Self other) const noexcept {
			return x * other.x - y * other.y;
		}

		constexpr auto distance_from(Self other) const noexcept {
			const Self difference = (*this - other);
			return difference.magnitude();
		}

		constexpr auto magnitude() const noexcept {
			if constexpr (std::same_as<T, double>)
				return hypot(x, y);
			else if constexpr (std::same_as<T, float>)
				return hypotf(x, y);
			else
				return hypotl(x, y);
		}

#pragma region Mutation

		constexpr void flip_over_x_axis() noexcept {
			y = -y;
		}

		constexpr void flip_over_y_axis() noexcept {
			x = -x;
		}

		template <std::floating_point U>
		constexpr void rotate(U angle) noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			const T xt = x;
			x = xt * c - y * s;
			y = xt * s + y * c;
		}

		template <std::floating_point U>
		constexpr void rotate(U angle, Self pivot) noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			const T xt = x - pivot.x;
			const T yt = y - pivot.y;
			x = xt * c - yt * s;
			y = xt * s + yt * c;
		}

		constexpr void rotate_45l() noexcept {
			const T xt = x;
			const T yt = y;
			x = Constants::square_root_of_2 * (xt - yt);
			y = Constants::square_root_of_2 * (xt + yt);
		}

		constexpr void rotate_90l() noexcept {
			const T xt = x;
			x = y;
			y = -xt;
		}

		constexpr void rotate_45r() noexcept {
			const T xt = x;
			const T yt = y;
			x = Constants::square_root_of_2 * (xt + yt);
			y = Constants::square_root_of_2 * (yt - xt);
		}

		constexpr void rotate_90r() noexcept {
			const T xt = x;
			x = -y;
			y = xt;
		}

#pragma endregion

#pragma region Operators

		constexpr Self operator+(Self other) const noexcept {
			return Self{ x + other.x, y + other.y };
		}

		constexpr Self operator/(Self other) const noexcept {
			return Self{ x / other.x, y / other.y };
		}

		constexpr Self operator*(Self other) const noexcept {
			return Self{ x * other.x, y * other.y };
		}

		constexpr Self operator-(Self other) const noexcept {
			return Self{ x - other.x, y - other.y };
		}

#pragma endregion

#pragma endregion

	};

}