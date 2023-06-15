#pragma once
#include "type_traits.h"

namespace evo
{
	template<typename... Types>
	struct TypePack {
		static constexpr size_t Size = sizeof...( Types );

		template<size_t I>
		using At = indexed_t<I, Types...>;

		template<typename T>
		static constexpr size_t IndexOf = index_of_type<T, Types...>();

		template<template<typename... Ts> typename T>
		using ConvertTo = T < Types... >;

		template<template<typename... Ts> typename T, template<typename Ta> typename U>
		using ConvertToWith = T<U<Types>...>;

		TypePack() = delete;
	};
}