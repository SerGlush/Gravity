#pragma once
#include "vector3.h"

namespace evo
{
	template<std::floating_point T>
	struct Ray {
		Vector3<T> origin;
		Vector3<T> direction;

		Ray() = default;
		constexpr Ray(const Vector3<T>& origin, const Vector3<T>& direction) noexcept
			: origin(origin), direction(direction.normalized()) { }

		constexpr Vector3 at(const T t) const noexcept {
			return origin + direction * t;
		}

		constexpr Ray operator -() const noexcept {
			return Ray(origin, -direction);
		}
	};
}