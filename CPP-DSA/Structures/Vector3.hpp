#pragma once

#include "../Algorithms/Math.hpp"

#include <concepts>
#include <iostream>

namespace DSA {

	template <typename T>
		requires std::is_arithmetic_v<T>
	struct Vector3 {

		enum class InterpolationType {

			LINEAR,
			QUADRATIC,
			CUBIC,
			QUINTIC

		};

		using Self = Vector3;

		T x, y, z;

#pragma region Methods

		[[nodiscard]] constexpr Self absolute() const noexcept {
			return Self{
				.x = Math::absolute_value(x),
				.y = Math::absolute_value(y),
				.z = Math::absolute_value(z)
			};
		}

		[[nodiscard]] constexpr Self maximum_between(const Self other) const noexcept {
			return Self{
				.x = x > other.x ? x : other.x,
				.y = y > other.y ? y : other.y,
				.z = z > other.z ? z : other.z
			};
		}

		[[nodiscard]] constexpr T maximum_component() const noexcept {
			const T xy = x > y ? x : y;
			return xy > z ? xy : z;
		}

		[[nodiscard]] constexpr Self minimum_between(const Self other) const noexcept {
			return Self{
				.x = x < other.x ? x : other.x,
				.y = y < other.y ? y : other.y,
				.z = z < other.z ? z : other.z
			};
		}

		[[nodiscard]] constexpr T minimum_component() const noexcept {
			const T xy = x < y ? x : y;
			return xy < z ? xy : z;
		}

		[[nodiscard]] constexpr Self sign_numbers() const noexcept {
			return Self{
				.x = Math::sign_number(x),
				.y = Math::sign_number(y),
				.z = Math::sign_number(z)
			};
		}

#pragma region Checks

		[[nodiscard]] constexpr bool is_approximately_equal_to(const Self other, const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Math::absolute_value(x - other.x) <= epsilon
				&& Math::absolute_value(y - other.y) <= epsilon
				&& Math::absolute_value(z - other.z) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_finite() const noexcept {
			return Math::is_finite(x)
				& Math::is_finite(y)
				& Math::is_finite(z);
		}

		[[nodiscard]] constexpr bool is_normalized(const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Math::absolute_value(squared_magnitude() - T(1)) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_parallel_to(const Self other, const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return cross_product(other).squared_magnitude() <= epsilon * epsilon;
		}

		[[nodiscard]] constexpr bool is_perpendicular_to(const Self other, const T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Math::absolute_value(dot_product(other)) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_zero() const noexcept {
			return (x == 0) & (y == 0) & (z == 0);
		}

#pragma endregion

#pragma region Space

#pragma region Products

		[[nodiscard]] constexpr Self cross_product(Self other) const noexcept {
			return Self{
				.x = y * other.z - z * other.y,
				.y = z * other.x - x * other.z,
				.z = x * other.y - y * other.x
			};
		}

		[[nodiscard]] constexpr T dot_product(Self other) const noexcept {
			return x * other.x + y * other.y + z * other.z;
		}

		[[nodiscard]] constexpr T triple_product(Self second, Self third) const noexcept {
			return dot_product(second.cross_product(third));
		}

#pragma endregion

#pragma region Magnitude

		[[nodiscard]] constexpr Self cardinal_magnitude() const noexcept {
			return Self{
				.x = x * (x > y && x > z),
				.y = y * (y > x && y > z),
				.z = z * (z > x && z > y)
			};
		}

		[[nodiscard]] constexpr Self cardinal_normal() const noexcept {
			return Self{
				.x = x > y && x > z,
				.y = y > x && y > z,
				.z = z > x && z > y
			};
		}

		[[nodiscard]] constexpr T chebyshev_magnitude() const noexcept {
			const T ax = Math::absolute_value(x);
			const T ay = Math::absolute_value(y);
			const T az = Math::absolute_value(z);
			const T xy = ax > ay ? ax : ay;
			return xy > az ? xy : az;
		}

		[[nodiscard]] constexpr auto distance_from(const Self other) const noexcept {
			return Math::square_root(squared_distance_from(other));
		}

		[[nodiscard]] constexpr auto magnitude() const noexcept {
			return Math::square_root(squared_magnitude());
		}

		[[nodiscard]] constexpr auto manhattan_magnitude() const noexcept {
			return Math::absolute_value(x)
				+ Math::absolute_value(y)
				+ Math::absolute_value(z);
		}

		[[nodiscard]] constexpr Self normalize() const noexcept {
			return *this / static_cast<T>(magnitude());
		}

		[[nodiscard]] constexpr auto squared_distance_from(const Self other) const noexcept {
			return Math::square(x - other.x) + Math::square(y - other.y) + Math::square(z - other.z);
		}

		[[nodiscard]] constexpr T squared_magnitude() const noexcept {
			return x * x + y * y + z * z;
		}

#pragma endregion

#pragma endregion

#pragma region Transformation

#pragma region Reflection

		[[nodiscard]] constexpr Self arbitrary_perpendicular() const noexcept {
			return cross_product(Math::absolute_value(x) < Math::absolute_value(y)
				? Self{ .x = 1, .y = 0, .z = 0 } : Self{ .x = 0, .y = 1, .z = 0 });
		}

		// Normal must be normalized.
		[[nodiscard]] constexpr Self reflect(const Self normal) const noexcept {
			return *this - normal * (T(2) * dot_product(normal));
		}

		[[nodiscard]] constexpr Self reflect_over_x_axis() const noexcept {
			return Self{ x, -y, -z };
		}

		[[nodiscard]] constexpr Self reflect_over_y_axis() const noexcept {
			return Self{ .x = -x, .y = y, .z = -z };
		}

		[[nodiscard]] constexpr Self reflect_over_z_axis() const noexcept {
			return Self{ .x = -x, .y = -y, .z = z };
		}

#pragma endregion

#pragma region Rotation

		[[nodiscard]] constexpr auto angle_to(const Self other) const noexcept {
			return Math::arccosine(dot_product(other) / (magnitude() * other.magnitude()));
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate_x(const U angle) const noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			return Self{
				.x = x,
				.y = y * c - z * s,
				.z = y * s + z * c
			};
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate_y(const U angle) const noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			return Self{
				.x = x * c + z * s,
				.y = y,
				.z = -x * s + z * c
			};
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate_z(const U angle) const noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			return Self{
				.x = x * c - y * s,
				.y = x * s + y * c,
				.z = z
			};
		}

		// Axis must be normalized.
		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate(const U angle, const Self axis) const noexcept {
			const auto c = Math::cosine(angle);
			const auto s = Math::sine(angle);
			return *this * c
				+ axis.cross_product(*this) * s
				+ axis * (axis.dot_product(*this) * (T(1) - c));
		}

#pragma endregion

#pragma region Translation

		[[nodiscard]] constexpr Self ceiling() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{
					.x = Math::ceiling(x),
					.y = Math::ceiling(y),
					.z = Math::ceiling(z)
				};
			else
				return *this;
		}

		[[nodiscard]] constexpr Self clamp_between(Self minimum, Self maximum) const noexcept {
			return maximum_between(minimum).minimum_between(maximum);
		}

		[[nodiscard]] constexpr Self floor() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{
					.x = Math::floor(x),
					.y = Math::floor(y),
					.z = Math::floor(z)
				};
			else
				return *this;
		}

		template <InterpolationType INTERPOLATION_TYPE = InterpolationType::LINEAR, std::floating_point U>
		[[nodiscard]] constexpr Self interpolate(const Self other, const U time) const noexcept {
			if constexpr (INTERPOLATION_TYPE == InterpolationType::QUADRATIC)
				time = time < U(0.5)
				? U(2) * time * time
				: U(1) - (-U(2) * time + U(2)) * (-U(2) * time + U(2)) / U(2);
			else if constexpr (INTERPOLATION_TYPE == InterpolationType::CUBIC)
				time *= time * (U(3) - U(2) * time);
			else if constexpr (INTERPOLATION_TYPE == InterpolationType::QUINTIC)
				time *= time * time * (time * (time * U(6) - U(15)) + U(10));

			return Self{
				.x = x + (other.x - x) * time,
				.y = y + (other.y - y) * time,
				.z = z + (other.z - z) * time
			};
		}

		[[nodiscard]] constexpr Self project_onto(const Self other) const noexcept {
			return other * (dot_product(other) / other.squared_magnitude());
		}

		[[nodiscard]] constexpr Self rejection_from(const Self other) const noexcept {
			return *this - project_onto(other);
		}

		[[nodiscard]] constexpr Self snap(const T xyzScalar = 1) const noexcept {
			return snap(xyzScalar, xyzScalar, xyzScalar);
		}

		[[nodiscard]] constexpr Self snap(const T xScalar, const T yScalar, const T zScalar) const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{
					.x = Math::round(x / xScalar) * xScalar,
					.y = Math::round(y / yScalar) * yScalar,
					.z = Math::round(z / zScalar) * zScalar
				};
			else
				return *this;
		}

		[[nodiscard]] constexpr Self truncate() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{
					.x = Math::truncate(x),
					.y = Math::truncate(y),
					.z = Math::truncate(z)
				};
			else
				return *this;
		}

#pragma endregion

#pragma endregion

#pragma region Operators

#pragma region Arithmetic

		[[nodiscard]] constexpr Self operator+(Self other) const noexcept {
			return Self{ .x = x + other.x, .y = y + other.y, .z = z + other.z };
		}

		[[nodiscard]] constexpr Self operator/(Self other) const noexcept {
			return Self{ .x = x / other.x, .y = y / other.y, .z = z / other.z };
		}

		[[nodiscard]] constexpr Self operator*(Self other) const noexcept {
			return Self{ .x = x * other.x, .y = y * other.y, .z = z * other.z };
		}

		[[nodiscard]] constexpr Self operator-(Self other) const noexcept {
			return Self{ .x = x - other.x, .y = y - other.y, .z = z - other.z };
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
			return Self{ -x, -y, -z };
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		[[nodiscard]] constexpr Self operator/(U divisor) const noexcept {
			return Self{ x / divisor, y / divisor, z / divisor };
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		[[nodiscard]] constexpr Self operator*(U multiplier) const noexcept {
			return Self{ x * multiplier, y * multiplier, z * multiplier };
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		constexpr void operator/=(U divisor) noexcept {
			*this = *this / divisor;
		}

		template <typename U>
			requires std::is_arithmetic_v<U>
		constexpr void operator*=(U multiplier) noexcept {
			*this = *this * multiplier;
		}

#pragma endregion

#pragma region Comparison

		[[nodiscard]] constexpr bool operator==(Self other) const noexcept {
			return (x == other.x) & (y == other.y) & (z == other.z);
		}

		[[nodiscard]] constexpr bool operator!=(Self other) const noexcept {
			return (x != other.x) | (y != other.y) | (z != other.z);
		}

#pragma endregion

		friend std::ostream& operator<<(std::ostream& ostream, const Self& vector) {
			ostream << "{ " << vector.x << ", " << vector.y << ", " << vector.z << " }";
			return ostream;
		}

#pragma endregion

#pragma endregion

		/*
		angle_between
		signed_angle_to
		lerp
		slerp
		distance_to_plane
		project_onto_plane
		clamp_magnitude
		orthogonalize
		*/

	};

}