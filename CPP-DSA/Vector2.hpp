#pragma once

#include "Math.hpp"

#include <concepts>
#include <iostream>

namespace DSA {

	template <typename T>
		requires std::is_arithmetic_v<T>
	struct Vector2 {

		enum class InterpolationType {

			LINEAR,
			QUADRATIC,
			CUBIC,
			QUINTIC

		};

		using Self = Vector2;

		T x, y;

#pragma region Methods

		[[nodiscard]] constexpr Self absolute() const noexcept {
			return Self{ .x = Math::absolute_value(x), .y = Math::absolute_value(y) };
		}

		[[nodiscard]] constexpr Self maximum_between(const Self other) const noexcept {
			return Self{ .x = x > other.x ? x : other.x, .y = y > other.y ? y : other.y };
		}

		[[nodiscard]] constexpr T maximum_component() const noexcept {
			return x > y ? x : y;
		}

		[[nodiscard]] constexpr Self minimum_between(const Self other) const noexcept {
			return Self{ .x = x < other.x ? x : other.x, .y = y < other.y ? y : other.y };
		}

		[[nodiscard]] constexpr T minimum_component() const noexcept {
			return x < y ? x : y;
		}

		[[nodiscard]] constexpr Self sign_numbers() const noexcept {
			return Self{ .x = Math::sign_number(x), .y = Math::sign_number(y) };
		}

#pragma region Checks

		[[nodiscard]] constexpr bool is_approximately_equal_to(const Self other, const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Math::absolute_value(x - other.x) <= epsilon
				&& Math::absolute_value(y - other.y) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_finite() const noexcept {
			return Math::is_finite(x) & Math::is_finite(y);
		}

		[[nodiscard]] constexpr bool is_normalized(const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Math::absolute_value(x * x + y * y - T(1)) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_parallel_to(const Self other, const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Math::absolute_value(cross_product(other)) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_perpendicular_to(const Self other, const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Math::absolute_value(dot_product(other)) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_zero() const noexcept {
			return (x == 0) & (y == 0);
		}

#pragma endregion

#pragma region Space

#pragma region Products

		[[nodiscard]] constexpr T cross_product(const Self other) const noexcept {
			return x * other.y - y * other.x;
		}

		[[nodiscard]] constexpr T dot_product(const Self other) const noexcept {
			return x * other.x + y * other.y;
		}

		[[nodiscard]] constexpr T triple_product(const Self other) const noexcept {
			const T cross = cross_product(other);
			return Self{ .x = -y * cross, .y = x * cross };
		}

#pragma endregion

#pragma region Magnitude

		[[nodiscard]] constexpr auto cardinal_magnitude() const noexcept {
			return Self{ .x = x * (x > y), .y = y * (y > x) };
		}

		[[nodiscard]] constexpr auto cardinal_normal() const noexcept {
			return Self{ .x = x > y, .y = y > x };
		}

		[[nodiscard]] constexpr auto chebyshev_magnitude() const noexcept {
			const T xt = Math::absolute_value(x);
			const T yt = Math::absolute_value(y);
			return xt > yt ? xt : yt;
		}

		[[nodiscard]] constexpr auto distance_from(const Self other) const noexcept {
			return Math::square_root(x - other.x, y - other.y);
		}

		[[nodiscard]] constexpr auto magnitude() const noexcept {
			return Math::hypotenuse(x, y);
		}

		[[nodiscard]] constexpr auto manhattan_magnitude() const noexcept {
			return -Math::absolute_value(x) + Math::absolute_value(y);
		}

		[[nodiscard]] constexpr Self normalize() const noexcept {
			return *this / static_cast<T>(magnitude());
		}

		[[nodiscard]] constexpr auto squared_distance_from(const Self other) const noexcept {
			return Math::square(x - other.x) + Math::square(y - other.y);
		}

		[[nodiscard]] constexpr T squared_magnitude() const noexcept {
			return x * x + y * y;
		}

#pragma endregion

#pragma region Transformation

#pragma region Reflection

		[[nodiscard]] constexpr Self perpendicular() const noexcept {
			return Self{ .x = -y, .y = x };
		}

		[[nodiscard]] constexpr Self reflect(Self normal) const noexcept {
			return *this - normal * (T{ 2 } * dot_product(normal));
		}

		[[nodiscard]] constexpr Self reflect_over_x_axis(Self normal) const noexcept {
			return Self{ .x = x, .y = -y };
		}

		[[nodiscard]] constexpr Self reflect_over_y_axis(Self normal) const noexcept {
			return Self{ .x = -x, .y = y };
		}

#pragma endregion

#pragma region Rotation

		[[nodiscard]] constexpr auto angle_to(const Self other) const noexcept {
			return Math::arctan2(cross_product(other), dot_product(other));
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate(const U angle) const noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			return Self{ .x = x * c - y * s, .y = x * s + y * c };
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate(const U angle, const Self pivot) const noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			const T xt = x - pivot.x;
			const T yt = y - pivot.y;
			return Self{ .x = pivot.x + xt * c - yt * s, .y = pivot.y + xt * s + yt * c };
		}

		[[nodiscard]] constexpr Self rotate_45l() const noexcept {
			return Self{ .x = (x - y) * Constants::inverse_square_root_of_2, .y = (x + y) * Constants::inverse_square_root_of_2 };
		}

		[[nodiscard]] constexpr Self rotate_90l() const noexcept {
			return Self{ .x = y, .y = -x };
		}

		[[nodiscard]] constexpr Self rotate_45r() const noexcept {
			return Self{ .x = (x + y) * Constants::inverse_square_root_of_2, .y = (y - x) * Constants::inverse_square_root_of_2 };
		}

		[[nodiscard]] constexpr Self rotate_90r() const noexcept {
			return Self{ .x = -y, .y = x };
		}

#pragma endregion

#pragma region Translation

		[[nodiscard]] constexpr Self ceiling() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{ .x = Math::ceiling(x), .y = Math::ceiling(y) };
			else
				return *this;
		}

		[[nodiscard]] constexpr Self clamp_between(const Self minimum, const Self maximum) const noexcept {
			return maximum_between(minimum).minimum_between(maximum);
		}

		[[nodiscard]] constexpr Self floor() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{ .x = Math::floor(x), .y = Math::floor(y) };
			else
				return *this;
		}

		template <InterpolationType INTERPOLATION_TYPE = InterpolationType::LINEAR, std::floating_point U>
		[[nodiscard]] constexpr Self interpolate(const Self other, const U time) const noexcept {
			if constexpr (INTERPOLATION_TYPE == InterpolationType::QUADRATIC)
				time = time < T(0.5)
				? T(2) * time * time
				: T(1) - (-T(2) * time + T(2)) * (-T(2) * time + T(2)) / T(2);
			else if constexpr (INTERPOLATION_TYPE == InterpolationType::CUBIC)
				time *= time * (T(3) - T(2) * time);
			else if constexpr (INTERPOLATION_TYPE == InterpolationType::QUINTIC)
				time *= time * time * (time * (time * T(6) - T(15)) + T(10));

			return Self{ .x = x + (other.x - x) * time, .y = y + (other.y - y) * time };
		}

		[[nodiscard]] constexpr Self project_onto(const Self other) const noexcept {
			return other * (dot_product(other) / other.squared_magnitude());
		}

		[[nodiscard]] constexpr Self rejection_from(const Self other) const noexcept {
			return *this - project_onto(other);
		}

		[[nodiscard]] constexpr Self snap(const T xyScalar = 1) const noexcept {
			return snap(xyScalar, xyScalar);
		}

		[[nodiscard]] constexpr Self snap(const T xScalar, const T yScalar) const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{ .x = Math::round(x / xScalar) * xScalar, .y = Math::round(y / yScalar) * yScalar };
			else
				return *this;
		}

		[[nodiscard]] constexpr Self truncate() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{ .x = Math::truncate(x), .y = Math::truncate(y) };
			else
				return *this;
		}

#pragma endregion

#pragma endregion

#pragma endregion

#pragma region Operators

#pragma region Arithmetic

		[[nodiscard]] constexpr Self operator+(Self other) const noexcept {
			return Self{ .x = x + other.x, .y = y + other.y };
		}

		[[nodiscard]] constexpr Self operator/(Self other) const noexcept {
			return Self{ .x = x / other.x, .y = y / other.y };
		}

		[[nodiscard]] constexpr Self operator*(Self other) const noexcept {
			return Self{ .x = x * other.x, .y = y * other.y };
		}

		[[nodiscard]] constexpr Self operator-(Self other) const noexcept {
			return Self{ .x = x - other.x, .y = y - other.y };
		}

		constexpr void operator+=(Self other) noexcept {
			*this = *this + other;
		}

		constexpr void operator/=(Self other) noexcept {
			*this = *this / other;
		}

		constexpr void operator*=(Self other) noexcept {
			*this = *this * other;
		}

		constexpr void operator-=(Self other) noexcept {
			*this = *this - other;
		}

		[[nodiscard]] constexpr Self operator-() const noexcept {
			return Self{ .x = -x, .y = -y };
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		[[nodiscard]] constexpr Self operator/(U divisor) const noexcept {
			return Self{ .x = x / divisor, .y = y / divisor };
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		[[nodiscard]] constexpr Self operator*(U multiplier) const noexcept {
			return Self{ .x = x * multiplier, .y = y * multiplier };
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		constexpr void operator/=(U divisor) const noexcept {
			*this = *this / divisor;
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		constexpr void operator*=(U multiplier) const noexcept {
			*this = *this * multiplier;
		}

#pragma endregion

#pragma region Comparison

		[[nodiscard]] constexpr bool operator==(Self other) const noexcept {
			return (x == other.x) & (y == other.y);
		}

		[[nodiscard]] constexpr bool operator!=(Self other) const noexcept {
			return (x != other.x) | (y != other.y);
		}

#pragma endregion

		friend std::ostream& operator<<(std::ostream& ostream, const Vector2 t) {
			ostream << "{ " << t.x << ", " << t.y << " }";
			return ostream;
		}

#pragma endregion

#pragma endregion

	};

}