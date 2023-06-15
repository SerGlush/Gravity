#pragma once
#include "vector2.h"
#include "circle.h"

namespace evo
{
	template<std::floating_point T>
	struct Rectangle {
		Vector2<T> a; //bottom-left corner
		Vector2<T> b; //upper-right corner

		Rectangle() noexcept = default;
		constexpr Rectangle(const Vector2<T> a, const Vector2<T> b) noexcept {
			const auto [m, M] = Vector2<T>::MinMax(a, b);
			this->a = m;
			this->b = M;
		}
		constexpr explicit Rectangle(const Circle<T>& circle) noexcept 
			: a(circle.center - circle.radius), b(circle.center + circle.radius) { }

		constexpr Vector2<T> upper_left() const noexcept {
			return { a.x, b.y };
		}
		constexpr Vector2<T> bottom_left() const noexcept {
			return a;
		}
		constexpr Vector2<T> upper_right() const noexcept {
			return b;
		}
		constexpr Vector2<T> bottom_right() const noexcept {
			return { b.x, a.y };
		}
		constexpr T left() const noexcept {
			return a.x;
		}
		constexpr T right() const noexcept {
			return b.x;
		}
		constexpr T bottom() const noexcept {
			return a.y;
		}
		constexpr T top() const noexcept {
			return b.y;
		}

		constexpr Vector2<T> center() const noexcept {
			return ( a + b ) * T(0.5l);
		}
		constexpr Vector2<T> size() const noexcept {
			return b - a;
		}

		constexpr bool contains(const Vector2<T> p) const noexcept {
			return a <= p && p <= b;
		}
		constexpr bool contains(const Rectangle<T>& other) const  noexcept {
			return a <= other.a && other.b <= b;
		}
		constexpr bool intersects(const Rectangle<T>& other) const noexcept {
			return a <= other.b && other.a <= b;
		}
		constexpr bool intersects(const Circle<T>& circle) const noexcept {
			return math::sqr(circle.radius) > ( Vector2<T>::Max(a, Vector2<T>::Min(circle.center, b)) - circle.center ).sqrlen();
		}
	};
}