#pragma once
#include <type_traits>
#include <concepts>
#include <array>
#include "type_traits.h"

namespace evo
{
	namespace detail
	{
		template<size_t S, typename T, size_t... Is>
		constexpr std::array<T, S> make_array_of_impl( const T& value, std::index_sequence<Is...> ) {
			return { { ( static_cast<void>( Is ), value )... } };
		};

		template<typename T, typename U, typename F, typename... Us, size_t... Is>
			constexpr std::array<T, sizeof...( Us )> make_array_impl(F&& f, std::index_sequence<Is...>, Us... list) {
			return std::array<T, sizeof...( Us )>{ { ( static_cast<void>( Is ), f(list, Is) )... } };
		}

		template<typename T, size_t Size>
		auto array_size_impl(const std::array<T, Size>&) -> std::integral_constant<size_t, Size>;
	}

	// returns array of copies of some value
	template<size_t S, typename T>
	inline constexpr std::array<T, S> make_array_of(const T& value) {
		return detail::make_array_of_impl<S, T>(value, std::make_index_sequence<S>{});
	}

	// returns array of results of F invokation with every element of list as first argument, and index as second
	template<typename T, typename U, typename F, typename... Us>
	//requires (std::convertible_to<Us, U> && ...) && std::is_invocable_r_v<T, F, U, size_t>
	constexpr std::array<T, sizeof...(Us)> make_array(F&& f, Us... list) {
		return detail::make_array_impl<T, U>(std::forward<F>(f), std::make_index_sequence<sizeof...( Us )>{}, list...);
	}

	template<typename Array>
	constexpr size_t array_size = decltype(detail::array_size_impl(std::declval<const Array&>()))::value;

	inline constexpr size_t hash_combine(size_t a, size_t b) {
		return a ^ ( b + 0x9e3779b9 + ( a << 6 ) + ( a >> 2 ) );
	}

	template<typename T>
	struct DefaultConstructor {
		void operator()(T* const ptr) const noexcept( std::is_nothrow_default_constructible_v<T> ) {
			std::construct_at(ptr);
		}
	};

	template<typename T>
	struct DefaultDestructor {
		void operator()(T* const ptr) const noexcept( std::is_nothrow_destructible_v<T> ) {
			std::destroy_at(ptr);
		}
	};

	template<typename... TArgs>
	struct NoopFunctor {
		void operator()(const TArgs&...) const noexcept { }
	};

	namespace detail { 
		template<typename... Args>
		struct NoopFnImpl {
			inline static void( *pointer )( Args... ) = [](Args... args) { };
		};
	}

	template<typename... Args>
	inline auto noop_fn_ptr = detail::NoopFnImpl<Args...>::pointer;

	struct Empty { };

	template<bool Test, typename T>
	using ConditionalField = std::conditional_t<Test, T, Empty>;
}