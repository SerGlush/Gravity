#pragma once
#include <type_traits>
#include <concepts>
#include <cstdint>

namespace evo
{
	template<typename T, typename... Ts>
	concept AllSame = ( std::same_as<T, Ts> && ... );

	template<typename T, typename... Ts>
	concept AnyOf = ( std::same_as<T, Ts> || ... );

	template<typename T>
	concept Ratio = requires( ) {
		{ T::num } -> std::integral;
		{ T::den } -> std::integral;
	};

	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template<typename T>
	concept NonBoolIntegral = std::integral<T> && !std::same_as<bool, T>;
}