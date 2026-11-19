#pragma once

#include "Vector2.hpp"

namespace DSA {

	template <typename T>
		requires std::is_arithmetic_v<T>
	struct Bounds2 {

		using Self = Bounds2<T>;
		using Vertex = Vector2<T>;

#pragma region Properties

		Vertex minimums;
		Vertex maximums;

#pragma endregion

#pragma region Methods

		[[nodiscard]] constexpr T area() const noexcept {
			const Vertex dimensions = maximums - minimums;
			return dimensions.x * dimensions.y;
		}

		[[nodiscard]] constexpr Vertex dimensions() const noexcept {
			return maximums - minimums;
		}

		[[nodiscard]] constexpr Vertex center() const noexcept {
			return (minimums + maximums) / T(2);
		}

		[[nodiscard]] constexpr bool contains(const Vertex& point) const noexcept {
			return point.x >= minimums.x && point.x <= maximums.x
				&& point.y >= minimums.y && point.y <= maximums.y;
		}

		[[nodiscard]] constexpr T distance_squared_from(const Vertex& point) const noexcept {
			const T dx = point.x < minimums.x ? minimums.x - point.x
				: point.x > maximums.x ? point.x - maximums.x : T(0);
			const T dy = point.y < minimums.y ? minimums.y - point.y
				: point.y > maximums.y ? point.y - maximums.y : T(0);
			return dx * dx + dy * dy;
		}

		[[nodiscard]] constexpr T enlargement(const Self& other) const noexcept {
			return merged_with(other).area() - area();
		}

		[[nodiscard]] constexpr bool intersects(const Self& other) const noexcept {
			return minimums.x <= other.maximums.x
				&& maximums.x >= other.minimums.x
				&& minimums.y <= other.maximums.y
				&& maximums.y >= other.minimums.y;
		}

		[[nodiscard]] constexpr Self merged_with(const Self& other) const noexcept {
			return Self{ minimums.minimum_between(other.minimums), maximums.maximum_between(other.maximums) };
		}

		[[nodiscard]] constexpr bool overlaps(const Self& other) const noexcept {
			return minimums.x < other.maximums.x
				&& maximums.x > other.minimums.x
				&& minimums.y < other.maximums.y
				&& maximums.y > other.minimums.y;
		}

#pragma endregion

	};

}