#pragma once

namespace crstl
{
	template<size_t N> struct fixed_length_type { typedef uint64_t type; };
	template<> struct fixed_length_type<1> { typedef uint8_t type; };
	template<> struct fixed_length_type<2> { typedef uint16_t type; };
	template<> struct fixed_length_type<4> { typedef uint32_t type; };
	template<size_t N> struct fixed_length_select_type { typedef typename fixed_length_type<N < 256 ? 1 : N < 65536 ? 2 : N < 4294967296 ? 4 : 8>::type type; };
};