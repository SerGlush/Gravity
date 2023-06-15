#pragma once
#include "vector2.h"

namespace evo
{
	template<std::floating_point T>
	struct Circle {
		Vector2<T> center;
		T radius;

		Circle() noexcept = default;
		constexpr Circle(const Vector2<T> center, const T radius) noexcept : center(center), radius(radius) { }

		constexpr bool contains(const Vector2<T>& p) const noexcept {
			return ( p - center ).sqrlen() <= math::sqr(radius);
		}
	};
}