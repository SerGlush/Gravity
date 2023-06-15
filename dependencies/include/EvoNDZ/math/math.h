#pragma once
#include <concepts>
#include <numeric>
#include <numbers>
#include <limits>
#include <cmath>
#include "../util/concepts.h"

namespace evo::math
{
	template<std::floating_point T>
	inline constexpr T two_pi = std::numbers::pi_v<T> * 2;

	template<std::floating_point T>
	inline constexpr T inv_2pi = std::numbers::inv_pi_v<T> * 0.5;
	
	template<std::signed_integral T>
	inline constexpr bool diffsign(T a, T b) noexcept {
		typedef std::make_unsigned_t<T> UT;
		constexpr UT signmask = UT(1) << UT(std::numeric_limits<UT>::digits - 1);
		return ( (UT)a ^ (UT)b ) & signmask;
	}

	template<Arithmetic T, 
		typename U = std::conditional_t<std::floating_point<T>, 
											T, 
											std::conditional_t<std::unsigned_integral<T>, std::uintmax_t, std::intmax_t>
		>>
	inline constexpr U ipow(T v, unsigned b) noexcept {

		U res = 1;
		U a = v;
		while (b) {
			if (b & 1) res *= a;
			a *= a;
			b >>= 1u;
		}
		return res;
	}

	template<Arithmetic TValue>
	inline constexpr TValue abs(TValue x) noexcept {
		if (std::is_constant_evaluated()) {
			if constexpr (std::floating_point<TValue>) {
				return x == TValue(0) ? TValue(0) : ( x > TValue(0) ? x : -x );
			}
			else {
				return x > TValue(0) ? x : -x;
			}
		}
		else {
			return std::abs(x);
		}
	}

	template<Arithmetic T>
	inline constexpr T sqr(T a) noexcept {
		return a * a;
	}

	template<std::floating_point TValue>
	inline constexpr TValue sqrt(TValue a) {
		if(std::is_constant_evaluated()) { 
			if (a <= TValue(0)) {
				if (a == TValue(0)) {
					return TValue(0);
				}
				return std::numeric_limits<TValue>::quiet_NaN();
			}
			TValue x = TValue(0.3) * a;
			TValue y;
			do {
				y = x;
				x = TValue(0.5) * ( x + a / x );
			}
			while (abs(x - y) >= std::numeric_limits<TValue>::epsilon());
			return x;
		}
		else {
			return std::sqrt(a);
		}
	}

	template<std::floating_point TValue>
	inline constexpr TValue cbrt(TValue a) noexcept {
		if (std::is_constant_evaluated()) {
			constexpr int iterations = 3 + 2 * same_as<double, TValue>;
			constexpr TValue d2b3 = TValue(2.0 / 3.0), d1b3 = TValue(1.0 / 3.0);
			if (a < TValue(0)) return -cbrt(-a);
			TValue r = a;
			int ex = 0;

			while (r < TValue(0.125)) { r *= TValue(8.0); --ex; }
			while (r > TValue(1.0)) { r *= TValue(0.125); ++ex; }

			r = ( -TValue(0.46946116) * r + TValue(1.072302) ) * r + TValue(0.3812513);

			while (ex < 0) { r *= TValue(0.5); ++ex; }
			while (ex > 0) { r *= TValue(2.0); --ex; }

			for (int i = 0; i < iterations; ++i) {
				r = d2b3 * r + d1b3 * a / ( r * r );
			}
			return r;
		}
		else {
			return std::cbrt(a);
		}
	}

	template<std::floating_point T>
	inline constexpr T trunc(T a) noexcept {
		if (std::is_constant_evaluated()) {
			if (abs(a) < std::numeric_limits<T>::epsilon()) return a;
			if (abs(a) >= std::numeric_limits<intmax_t>::max()) return a;
			return (T)(intmax_t)a;
		}
		else {
			return std::trunc(a);
		}
	}

	template<std::integral T>
	inline constexpr T trunc_to(T a, std::make_unsigned_t<T> b) noexcept {
		return ( a / T(b) ) * T(b);
	}

	template<std::signed_integral T>
	inline constexpr T ceil_to(T a, std::make_unsigned_t<T> b) noexcept {
		return trunc_to(a > 0 ? a + T(b) - 1 : a, b);
	}

	template<std::unsigned_integral T>
	inline constexpr T ceil_to(T a, T b) noexcept {
		return trunc_to(a + b - 1, b);
	}

	template<std::signed_integral T>
	inline constexpr T floor_to(T a, std::make_unsigned_t<T> b) noexcept {
		return trunc_to(a < 0 ? a - T(b) + 1 : a, b);
	}

	template<std::unsigned_integral T>
	inline constexpr T floor_to(T a, T b) noexcept {
		return trunc_to(a, b);
	}

	template<std::floating_point T>
	inline constexpr T ceil(T a) noexcept {
		if (std::is_constant_evaluated()) {
			T i = trunc(a);
			return i + ( ( a > T(0) ) && ( i < a ) );
		}
		else {
			return std::ceil(a);
		}
	}

	template<std::floating_point T>
	inline constexpr T floor(T a) noexcept {
		if(std::is_constant_evaluated()) { 
			T i = trunc(a);
			return i - ( ( a < T(0) ) && ( a < i ) );
		}
		else {
			return std::floor(a);
		}
	}

	template<std::floating_point T>
	inline constexpr T fmod(T a, T b) noexcept {
		if (std::is_constant_evaluated()) {
			return a - trunc(a / b) * b;
		}
		else {
			return std::fmod(a, b);
		}
	}

	// converts any angle to the range of [0, 2pi]
	template<std::floating_point T>
	inline constexpr T normalize_angle(T a) noexcept {
		T m = fmod(a, two_pi<T>);
		return m >= T(0) ? m : ( m + two_pi<T> );
	}

	template<std::unsigned_integral T>
	inline constexpr T factorial(T a) {
		constexpr T ft[] { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600 };
		return a < T(13) ? ft[a] : ( a * factorial(a - T(1)) );
	}

	template<std::floating_point T>
	inline constexpr T factorial(unsigned a) {
		T r { 1 };
		while (a > 1) {
			r *= T { a };
			--a;
		}
		return r;
	}

	template<std::unsigned_integral T>
	inline constexpr T log2(T x) noexcept {
		T r = 0;
		while (x >>= 1) {
			++r;
		}
		return r;
	}

	// (511, 10) -> 2; (512, 10) -> 3
	template<std::unsigned_integral T>
	inline constexpr T log(T a, T base) noexcept {
		return log2(a) / log2(base); 
	}

	namespace detail
	{
		template<std::floating_point T>
		inline constexpr T log_(T x) {
			T d { 1 };
			x -= d;
			T px = x;
			T r = x;
			T p1 = d;
			T prev = std::numeric_limits<T>::lowest();
			while (abs(r - prev) > std::numeric_limits<T>::epsilon()) {
				px *= x;
				d += T { 1 };
				p1 *= -T{ 1 };
				prev = r;
				r += p1 * px / d;
			}
			return r;
		}
	}

	template<std::floating_point T>
	inline constexpr T log(T x) {
		if (std::is_constant_evaluated()) {
			T r { 0 };
			while (x > 1) {
				r += std::numbers::ln2_v<T>;
				x *= 0.5f;
			}
			return r + detail::log_(x);
		}
		else {
			return std::log(x);
		}
	}

	template<std::floating_point T>
	inline constexpr T exp(T x) noexcept {
		T r = T{ 1 } + x;
		T prev { 1 };
		T px = x;
		T dn = 1;
		T dv = 1;
		while (abs(r - prev) > std::numeric_limits<T>::epsilon()) {
			px *= x;
			dn += T{1};
			dv *= dn;
			prev = r;
			r += px / dv;
		}
		return r;
	}

	template<std::floating_point T>
	inline constexpr T pow(T a, T x) {
		if (std::is_constant_evaluated()) {
			return exp(x * log(a));
		}
		else {
			return std::pow(a, x);
		}
	}

	template<std::floating_point T>
	inline constexpr T sin(T x) noexcept {
		if(std::is_constant_evaluated()) { 
			x = x - two_pi<T> * int(x * inv_2pi<T>);
			T prev { 0 };
			T r = x;
			T dn { 1 };
			T dv { 1 };
			T s { 1 };
			const T sqrx = x * x;
			T px = x;
			while (abs(r - prev) > std::numeric_limits<T>::epsilon()) {
				s *= -T{ 1 };
				px *= sqrx;
				dn += T{2};
				dv *= dn * (dn - 1);
				prev = r;
				r += s * px / dv;
			}
			return r;
		}
		else {
			return std::sin(x);
		}
	}

	template<std::floating_point T>
	inline constexpr T cos(T x) noexcept {
		if (std::is_constant_evaluated()) {
			x = x - two_pi<T> *int(x * inv_2pi<T>);
			T prev { 0 };
			T r = 1;
			T dn { 0 };
			T dv { 1 };
			T s { 1 };
			const T sqrx = x * x;
			T px { 1 };
			while (abs(r - prev) > std::numeric_limits<T>::epsilon()) {
				s *= -T { 1 };
				px *= sqrx;
				dn += T { 2 };
				dv *= dn * ( dn - 1 );
				prev = r;
				r += s * px / dv;
			}
			return r;
		}
		else {
			return std::cos(x);
		}
	}

	template<std::floating_point T>
	inline constexpr T atan(T x) {
		if (std::is_constant_evaluated()) {
			if (x > T { 1 }) return std::numbers::pi_v<T> *T { 0.5 } - atan(T{1} / x);
			T prev { 0.f };
			T r = x;
			T dn { 1 };
			T s { 1 };
			const T sqrx = x * x;
			T px = x;
			while (abs(r - prev) > std::numeric_limits<T>::epsilon()) {
				s *= -T { 1 };
				px *= sqrx;
				dn += T { 2 };
				prev = r;
				r += s * px / dn;
			}
			return r;
		}
		else {
			return std::atan(x);
		}
	}

	template<std::floating_point T>
	inline constexpr T atan2(T y, T x) {
		if (std::is_constant_evaluated()) {
			if (x > T { 0 }) return atan(y / x);
			if (x < T { 0 }) {
				if (y >= T { 0 })
					return atan(y / x) + std::numbers::pi_v<T>;
				else
					return atan(y / x) - std::numbers::pi_v<T>;
			}
			if (y > T { 0 }) return std::numbers::pi_v<T> *T { 0.5 };
			if (y < T { 0 }) return -std::numbers::pi_v<T> *T { 0.5 };
			return std::numeric_limits<T>::quiet_NaN();
		}
		else {
			return std::atan2(y, x);
		}
	}
}