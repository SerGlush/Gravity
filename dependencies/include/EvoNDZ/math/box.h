#pragma once
#include <algorithm>
#include "vector3.h"
#include "ray.h"
#include "sphere.h"

namespace evo
{
	template<std::floating_point T>
	struct Box {
		Vector3<T> a; // min
		Vector3<T> b; // max

		Box() noexcept = default;
		constexpr Box(const Vector3<T>& a, const Vector3<T>& b) noexcept {
			const auto [m, M] = Vector3<T>::MinMax(a, b);
			this->a = m;
			this->b = M;
		}
		constexpr explicit Box(const Sphere& sphere) noexcept 
			: a(sphere.center - sphere.radius), b(sphere.center + sphere.radius) { }

		constexpr Vector3<T> size() const noexcept {
			return b - a;
		}
		constexpr Vector3<T> center() const noexcept {
			return ( a + b ) * 0.5f;
		}

		constexpr bool contains(const Vector3<T>& p) const noexcept {
			return a <= p && p <= b;
		}
		constexpr bool contains(const Box<T>& other) const noexcept {
			return a <= other.a && other.b <= b;
		}
		constexpr bool intersects(const Box<T>& other) const {
			return a <= other.b && other.a <= b;
		}
		constexpr bool intersects(const Sphere<T>& sphere) const noexcept {
			return math::sqr(sphere.radius) > ( Vector3<T>::Max(a, Vector3<T>::Min(sphere.center, b)) - sphere.center).sqrlen();
		}
		constexpr T intersects(const Ray<T>& ray) const noexcept {
			Vector3<T> t1 = ( a - ray.origin ) / ray.direction;
			Vector3<T> t2 = ( b - ray.origin ) / ray.direction;
			const auto [tmin, tmax] = Vector3<T>::MinMax(t1, t2);
			return tmin.max() <= tmax.min();
		}
	};
}