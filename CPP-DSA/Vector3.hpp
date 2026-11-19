#pragma once

#include "Math.hpp"

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

		using Self = Vector3<T>;

		T x, y, z;

#pragma region Methods

		[[nodiscard]] constexpr Self absolute() const noexcept {
			return Self{
				Algorithms::Math::absolute_value(x),
				Algorithms::Math::absolute_value(y),
				Algorithms::Math::absolute_value(z)
			};
		}

		[[nodiscard]] constexpr Self maximum_between(Self other) const noexcept {
			return Self{
				x > other.x ? x : other.x,
				y > other.y ? y : other.y,
				z > other.z ? z : other.z
			};
		}

		[[nodiscard]] constexpr T maximum_component() const noexcept {
			const T xy = x > y ? x : y;
			return xy > z ? xy : z;
		}

		[[nodiscard]] constexpr Self minimum_between(Self other) const noexcept {
			return Self{
				x < other.x ? x : other.x,
				y < other.y ? y : other.y,
				z < other.z ? z : other.z
			};
		}

		[[nodiscard]] constexpr T minimum_component() const noexcept {
			const T xy = x < y ? x : y;
			return xy < z ? xy : z;
		}

		[[nodiscard]] constexpr Self sign_numbers() const noexcept {
			return Self{
				Algorithms::Math::sign_number(x),
				Algorithms::Math::sign_number(y),
				Algorithms::Math::sign_number(z)
			};
		}

#pragma region Checks

		[[nodiscard]] constexpr bool is_approximately_equal_to(Self other, T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Algorithms::Math::absolute_value(x - other.x) <= epsilon
				&& Algorithms::Math::absolute_value(y - other.y) <= epsilon
				&& Algorithms::Math::absolute_value(z - other.z) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_finite() const noexcept {
			return Algorithms::Math::is_finite(x)
				& Algorithms::Math::is_finite(y)
				& Algorithms::Math::is_finite(z);
		}

		[[nodiscard]] constexpr bool is_normalized(T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Algorithms::Math::absolute_value(squared_magnitude() - T(1)) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_parallel_to(Self other, T epsilon = static_cast<T>(1e-5)) const noexcept {
			return cross_product(other).squared_magnitude() <= epsilon * epsilon;
		}

		[[nodiscard]] constexpr bool is_perpendicular_to(Self other, T epsilon = static_cast<T>(1e-5)) const noexcept {
			return Algorithms::Math::absolute_value(dot_product(other)) <= epsilon;
		}

		[[nodiscard]] constexpr bool is_zero() const noexcept {
			return (x == 0) & (y == 0) & (z == 0);
		}

#pragma endregion

#pragma region Space

#pragma region Products

		[[nodiscard]] constexpr Self cross_product(Self other) const noexcept {
			return Self{
				y * other.z - z * other.y,
				z * other.x - x * other.z,
				x * other.y - y * other.x
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
				x * (x > y && x > z),
				y * (y > x && y > z),
				z * (z > x && z > y)
			};
		}

		[[nodiscard]] constexpr Self cardinal_normal() const noexcept {
			return Self{
				x > y && x > z,
				y > x && y > z,
				z > x && z > y
			};
		}

		[[nodiscard]] constexpr T chebyshev_magnitude() const noexcept {
			const T ax = Algorithms::Math::absolute_value(x);
			const T ay = Algorithms::Math::absolute_value(y);
			const T az = Algorithms::Math::absolute_value(z);
			const T xy = ax > ay ? ax : ay;
			return xy > az ? xy : az;
		}

		[[nodiscard]] constexpr auto distance_from(Self other) const noexcept {
			return Algorithms::Math::square_root(squared_distance_from(other));
		}

		[[nodiscard]] constexpr auto magnitude() const noexcept {
			return Algorithms::Math::square_root(squared_magnitude());
		}

		[[nodiscard]] constexpr auto manhattan_magnitude() const noexcept {
			return Algorithms::Math::absolute_value(x)
				+ Algorithms::Math::absolute_value(y)
				+ Algorithms::Math::absolute_value(z);
		}

		[[nodiscard]] constexpr Self normalize() const noexcept {
			return *this / static_cast<T>(magnitude());
		}

		[[nodiscard]] constexpr auto squared_distance_from(Self other) const noexcept {
			const auto dx = x - other.x;
			const auto dy = y - other.y;
			const auto dz = z - other.z;
			return dx * dx + dy * dy + dz * dz;
		}

		[[nodiscard]] constexpr T squared_magnitude() const noexcept {
			return x * x + y * y + z * z;
		}

#pragma endregion

#pragma endregion

#pragma region Transformation

#pragma region Reflection

		[[nodiscard]] constexpr Self arbitrary_perpendicular() const noexcept {
			return cross_product(Algorithms::Math::absolute_value(x) < Algorithms::Math::absolute_value(y) ? Self{ 1, 0, 0 } : Self{ 0, 1, 0 });
		}

		// Normal must be normalized.
		[[nodiscard]] constexpr Self reflect(Self normal) const noexcept {
			return *this - normal * (T(2) * dot_product(normal));
		}

		[[nodiscard]] constexpr Self reflect_over_x_axis() const noexcept {
			return Self{ x, -y, -z };
		}

		[[nodiscard]] constexpr Self reflect_over_y_axis() const noexcept {
			return Self{ -x, y, -z };
		}

		[[nodiscard]] constexpr Self reflect_over_z_axis() const noexcept {
			return Self{ -x, -y, z };
		}

#pragma endregion

#pragma region Rotation

		[[nodiscard]] constexpr auto angle_to(Self other) const noexcept {
			return Algorithms::Math::arccosine(dot_product(other) / (magnitude() * other.magnitude()));
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate_x(U angle) const noexcept {
			const auto c = Algorithms::Math::cosine(angle);
			const auto s = Algorithms::Math::sine(angle);
			return Self{
				x,
				y * c - z * s,
				y * s + z * c
			};
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate_y(U angle) const noexcept {
			const auto c = Algorithms::Math::cosine(angle);
			const auto s = Algorithms::Math::sine(angle);
			return Self{
				x * c + z * s,
				y,
				-x * s + z * c
			};
		}

		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate_z(U angle) const noexcept {
			const auto c = Algorithms::Math::cosine(angle);
			const auto s = Algorithms::Math::sine(angle);
			return Self{
				x * c - y * s,
				x * s + y * c,
				z
			};
		}

		// Axis must be normalized.
		template <std::floating_point U>
		[[nodiscard]] constexpr Self rotate(U angle, Self axis) const noexcept {
			const auto c = Algorithms::Math::cosine(angle);
			const auto s = Algorithms::Math::sine(angle);
			return *this * c
				+ axis.cross_product(*this) * s
				+ axis * (axis.dot_product(*this) * (T(1) - c));
		}

#pragma endregion

#pragma region Translation

		[[nodiscard]] constexpr Self ceiling() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{
					Algorithms::Math::ceiling(x),
					Algorithms::Math::ceiling(y),
					Algorithms::Math::ceiling(z)
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
					Algorithms::Math::floor(x),
					Algorithms::Math::floor(y),
					Algorithms::Math::floor(z)
			};
			else
				return *this;
		}

		template <InterpolationType INTERPOLATION_TYPE = InterpolationType::LINEAR, std::floating_point U>
		[[nodiscard]] constexpr Self interpolate(Self other, U time) const noexcept {
			if constexpr (INTERPOLATION_TYPE == InterpolationType::QUADRATIC)
				time = time < U(0.5)
				? U(2) * time * time
				: U(1) - ((-U(2) * time + U(2)) * (-U(2) * time + U(2))) / U(2);
			else if constexpr (INTERPOLATION_TYPE == InterpolationType::CUBIC)
				time *= time * (U(3) - U(2) * time);
			else if constexpr (INTERPOLATION_TYPE == InterpolationType::QUINTIC)
				time *= time * time * (time * (time * U(6) - U(15)) + U(10));

			return Self{
				x + (other.x - x) * time,
				y + (other.y - y) * time,
				z + (other.z - z) * time
			};
		}

		[[nodiscard]] constexpr Self project_onto(Self other) const noexcept {
			return other * (dot_product(other) / other.squared_magnitude());
		}

		[[nodiscard]] constexpr Self rejection_from(Self other) const noexcept {
			return *this - project_onto(other);
		}

		[[nodiscard]] constexpr Self snap(T xyzScalar = 1) const noexcept {
			return snap(xyzScalar, xyzScalar, xyzScalar);
		}

		[[nodiscard]] constexpr Self snap(T xScalar, T yScalar, T zScalar) const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{
					Algorithms::Math::round(x / xScalar) * xScalar,
					Algorithms::Math::round(y / yScalar) * yScalar,
					Algorithms::Math::round(z / zScalar) * zScalar
				};
			else
				return *this;
		}

		[[nodiscard]] constexpr Self truncate() const noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return Self{
					Algorithms::Math::truncate(x),
					Algorithms::Math::truncate(y),
					Algorithms::Math::truncate(z)
				};
			else
				return *this;
		}

#pragma endregion

#pragma endregion

#pragma region Operators

#pragma region Arithmetic

		[[nodiscard]] constexpr Self operator+(Self other) const noexcept {
			return Self{ x + other.x, y + other.y, z + other.z };
		}

		[[nodiscard]] constexpr Self operator/(Self other) const noexcept {
			return Self{ x / other.x, y / other.y, z / other.z };
		}

		[[nodiscard]] constexpr Self operator*(Self other) const noexcept {
			return Self{ x * other.x, y * other.y, z * other.z };
		}

		[[nodiscard]] constexpr Self operator-(Self other) const noexcept {
			return Self{ x - other.x, y - other.y, z - other.z };
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