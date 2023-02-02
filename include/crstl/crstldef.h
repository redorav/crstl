#pragma once

#include "config.h"

namespace crstl
{
	typedef decltype(nullptr)   nullptr_t;

	typedef decltype(sizeof(1)) size_t;

	typedef signed char int8_t;
	typedef unsigned char uint8_t;

	typedef signed short int16_t;
	typedef unsigned short uint16_t;

	typedef decltype(-2147483647) int32_t;
	typedef decltype(4294967295u) uint32_t;

	typedef decltype(-9223372036854775807) int64_t;
	typedef decltype(9223372036854775808u) uint64_t;

	static_assert(sizeof( int8_t) == 1, "Incorrect size for int8_t");
	static_assert(sizeof(uint8_t) == 1, "Incorrect size for uint8_t");
	static_assert(sizeof( int16_t) == 2, "Incorrect size for int16_t");
	static_assert(sizeof(uint16_t) == 2, "Incorrect size for uint16_t");
	static_assert(sizeof( int32_t) == 4, "Incorrect size for int32_t");
	static_assert(sizeof(uint32_t) == 4, "Incorrect size for uint32_t");
	static_assert(sizeof( int64_t) == 8, "Incorrect size for int64_t");
	static_assert(sizeof(uint64_t) == 8, "Incorrect size for uint64_t");
};