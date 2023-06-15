#pragma once
#include "vector3.h"

namespace evo
{
	template<std::floating_point T>
	struct Sphere {
		Vector3<T> center;
		T radius;

		Sphere() noexcept = default;
		constexpr Sphere(const Vector3<T>& center, const T radius) noexcept : center(center), radius(radius) { }

		constexpr bool contains(const Vector3<T>& p) const noexcept {
			return ( p - center ).sqrlen() <= math::sqr(radius);
		}
	};
}