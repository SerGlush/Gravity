#pragma once
#include <cstdint>
#include <limits>
#include "../math/math.h"

namespace evo
{
	namespace detail
	{
		template<unsigned Bits> struct int_capable_of_impl : 
			int_capable_of_impl<(Bits + std::numeric_limits<unsigned char>::digits)> { };

		struct int_capable_of_impl<( std::numeric_limits<unsigned char>::digits			)>	{ using type = signed char;			};
		struct int_capable_of_impl<( std::numeric_limits<unsigned short>::digits		)>	{ using type = signed short;		};
		struct int_capable_of_impl<( std::numeric_limits<unsigned int>::digits			)>	{ using type = signed int;			};
		struct int_capable_of_impl<( std::numeric_limits<unsigned long>::digits			)>	{ using type = signed long;			};
		struct int_capable_of_impl<( std::numeric_limits<unsigned long long>::digits	)>	{ using type = signed long long;	};

		template<unsigned Bits> struct uint_capable_of_impl :
			uint_capable_of_impl<( Bits + std::numeric_limits<unsigned char>::digits )> { };

		struct uint_capable_of_impl<( std::numeric_limits<unsigned char>::digits		)>	{ using type = unsigned char;		};
		struct uint_capable_of_impl<( std::numeric_limits<unsigned short>::digits		)>	{ using type = unsigned short;		};
		struct uint_capable_of_impl<( std::numeric_limits<unsigned int>::digits			)>	{ using type = unsigned int;		};
		struct uint_capable_of_impl<( std::numeric_limits<unsigned long>::digits		)>	{ using type = unsigned long;		};
		struct uint_capable_of_impl<( std::numeric_limits<unsigned long long>::digits	)>	{ using type = unsigned long long;	};
	}

	template<intmax_t Value>
	using int_capable_of = detail::int_capable_of_impl<( 
		math::ceil_to( math::log2(uintmax_t(Value < 0 ? ~Value : Value)) + 2u,
						(uintmax_t)std::numeric_limits<unsigned char>::digits )
	)>::type;

	template<uintmax_t Value>
	using uint_capable_of = detail::uint_capable_of_impl<( 
		math::ceil_to(math::log2(Value) + 1u, (uintmax_t)std::numeric_limits<unsigned char>::digits) 
	)>::type;
}