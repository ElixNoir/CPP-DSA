#pragma once

#pragma region Dependencies

#include "Point.hpp"

#pragma endregion

struct AxisAlignedBoundingBox {

    float X;
    float Y;
    float HalfWidth;
    float HalfHeight;

    [[nodiscard]] constexpr bool contains(const Point& p) const noexcept {
        return p.X >= X - HalfWidth && p.X <= X + HalfWidth && p.Y >= Y - HalfHeight && p.Y <= Y + HalfHeight;
    }

    [[nodiscard]] constexpr bool intersects(const AxisAlignedBoundingBox& other) const noexcept {
        return !(other.X - other.HalfWidth > X + HalfWidth
            || other.X + other.HalfWidth < X - HalfWidth
            || other.Y - other.HalfHeight > Y + HalfHeight
            || other.Y + other.HalfHeight < Y - HalfHeight);
    }

};

using AABB = AxisAlignedBoundingBox;
