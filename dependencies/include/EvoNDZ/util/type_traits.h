#pragma once
#include <type_traits>

namespace evo
{
	template<typename T, typename... Ts>
	constexpr size_t index_of_type() {
		size_t i = 0;
		bool found = false;
		( ( found |= std::is_same_v<T, Ts>, i += !found ), ... );
		return i;
	}

	namespace detail { 

		template<size_t I, typename TE, typename... Ts>
		struct indexed_type_impl {
			static_assert( I < sizeof...( Ts ), "Index out of range." );
		};

		template<size_t I, typename Tc, typename... Ts>
		struct indexed_type_impl<I, std::enable_if_t<( I == 0 )>, Tc, Ts...> {
			typedef Tc type;
		};

		template<size_t I, typename Tc, typename... Ts>
		struct indexed_type_impl<I, std::enable_if_t<( I > 0 && I < sizeof...(Ts) )>, Tc, Ts...> {
			typedef typename indexed_type_impl<I - 1, Ts...>::type type;
		};

		template<typename T, template<typename...> typename TTemplate >
		struct is_specialization : std::false_type { };

		template<template<typename...> typename Template, typename... TArgs>
		struct is_specialization<Template<TArgs...>, Template> : std::true_type { };

	}

	template<size_t I, typename... Ts>
	using indexed_t = typename detail::indexed_type_impl<I, void, Ts...>::type;

	template<typename T, template<typename...> typename TTemplate>
	constexpr bool is_specialization = detail::is_specialization<T, TTemplate>::value;

	template<typename T>
	using optimal_pass_t = std::conditional_t<std::is_trivial_v<T>, T, const T&>;
}