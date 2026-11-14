#pragma once

#include "Math.hpp"
#include "Vector2.hpp"

namespace Algorithms {

	namespace Geometry {

		using DSA::Vector2;

#pragma region Area

		constexpr static double area_of_circle(double radius) noexcept {
			return radius * radius * Constants::pi;
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static double area_of_polygon_signed(ITERATOR begin, ITERATOR end) noexcept {
			double sum = 0;
			Vector2<T>& point = *begin;
			Vector2<T>& previousPoint = point;
			for (auto iterator = std::next(begin); iterator != end; ++iterator) {
				point = *iterator;
				sum += point.x * point.y - previousPoint.x * previousPoint.y;
				previousPoint = point;
			}
			point = *begin;
			return (sum + point.x * point.y - previousPoint.x * previousPoint.y) / 2;
		}

		template <typename T>
		constexpr static T area_of_rectangle(T width, T length) noexcept {
			return width * length;
		}

		template <typename T>
		constexpr static T area_of_square(T width) noexcept {
			return width * width;
		}

		template <typename T>
		constexpr static T area_of_triangle_signed(Vector2<T> vertex1, Vector2<T> vertex2, Vector2<T> vertex3) noexcept {
			return ((vertex2.x - vertex1.x) * (vertex3.y - vertex1.y)
				- (vertex2.y - vertex1.y) * (vertex2.x - vertex1.x)) / 2;
		}

		template <typename T>
		constexpr static T area_of_triangle(T sideLength1, T sideLength2, T sideLength3) noexcept {
			const T s = (sideLength1 + sideLength2 + sideLength3) / 2;
			return Math::square_root(s * (s - sideLength1) * (s - sideLength2) * (s - sideLength3));
		}

#pragma endregion

#pragma region Perimeter

		constexpr static double circumference_of_circle(double diameter) noexcept {
			return diameter * Constants::pies;
		}

		template <typename T, std::input_iterator ITERATOR>
		constexpr static double perimeter_of_polygon(ITERATOR begin, ITERATOR end) noexcept {
			double sum = 0;
			for (auto iterator = std::next(begin); iterator != end; ++iterator)
				sum += hypotenuse(*iterator);
			return sum;
		}

		template <typename T>
		constexpr static T perimeter_of_rectangle(T width, T length) noexcept {
			return (width + length) * 2;
		}

		template <typename T>
		constexpr static T perimeter_of_square(T width) noexcept {
			return width * 4;
		}

		template <typename T>
		constexpr static T perimeter_of_triangle(T sideLength1, T sideLength2, T sideLength3) noexcept {
			return sideLength1 + sideLength2 + sideLength3;
		}

#pragma endregion

#pragma region Lines

		template <typename T>
		constexpr static Vector2<T> get_u(
			Vector2<T> a1, Vector2<T> a2,
			Vector2<T> b1, Vector2<T> b2,
			T denominator
		) noexcept {
			const T ua = ((b2.x - b1.x) * (a1.y - b1.y)
				- (b2.y - b1.y) * (a1.x - b1.x)) / denominator;
			const T ub = ((a2.x - a1.x) * (a1.y - b1.y)
				- (a2.y - a1.y) * (a1.x - b1.x)) / denominator;
			return Vector2<T>{ ua, ub };
		}

		template <typename T>
		constexpr static T lines_intersect(
			Vector2<T> a1, Vector2<T> a2,
			Vector2<T> b1, Vector2<T> b2
		) noexcept {
			return (b2.y - b1.y) * (a2.x - a1.x)
				- (b2.x - b1.x) * (a2.y - a1.y);
		}

		template <typename T>
		constexpr static Vector2<T> lines_intersection(
			Vector2<T> a1, Vector2<T> a2,
			Vector2<T> b1, Vector2<T> b2,
			T ua, T ub
		) noexcept {
			return Vector2<T>{
				a1.x + ua * (a2.x - a1.x),
				a1.y + ub * (a2.y - a1.y)
			};
		}

		template <typename T>
		[[nodiscard]] constexpr static bool line_segments_intersect(T ua, T ub) noexcept {
			return ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1;
		}

#pragma endregion

	}

}