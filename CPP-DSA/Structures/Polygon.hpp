#pragma once

#include "Container.hpp"
#include "../Vector2.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T>
		requires std::is_arithmetic_v<T>
	class Polygon : public ContainerPolicy::template STRUCTURE<Vector2<T>> {
	public:

		using Base = ContainerPolicy::template STRUCTURE<Vector2<T>>;
		using typename Base::INDEX;

		using Vertex = Vector2<T>;

#pragma region Methods

		[[nodiscard]] constexpr T distance_between(Vertex point, INDEX count) const noexcept {
			const T x = point.x, y = point.y;

			bool inside = false;
			T minimumDistanceSquared = INFINITY;

			const T* const data = Base::get_data();
			for (INDEX i = 0, j = count - 1; i < count; j = i++) {
				T xi = data[i].x, yi = data[i].y;
				T xj = data[j].x, yj = data[j].y;

				if (((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi))
					inside = !inside;

				T dx = xi - xj, dy = yi - yj;
				if (dx != 0.0 || dy != 0.0) {
					T t = ((x - xj) * dx + (y - yj) * dy) / (dx * dx + dy * dy);
					t = std::clamp(t, 0.0, 1.0);
					T projX = xj + t * dx;
					T projY = yj + t * dy;
					T distanceSquared = (x - projX) * (x - projX) + (y - projY) * (y - projY);
					minimumDistanceSquared = minimumDistanceSquared < distanceSquared ? minimumDistanceSquared : distanceSquared;
				}
			}

			T distance = Algorithms::Math::square_root(minimumDistanceSquared);
			return inside ? distance : -distance;
		}

#pragma endregion

	};

}