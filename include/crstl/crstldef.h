#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

namespace crstl
{
	typedef signed char int8_t;
	typedef unsigned char uint8_t;

	typedef signed short int16_t;
	typedef unsigned short uint16_t;

	typedef decltype(-2147483647) int32_t;
	typedef decltype(4294967295u) uint32_t;

	typedef decltype(-9223372036854775807) int64_t;
	typedef decltype(9223372036854775808u) uint64_t;

	typedef decltype(nullptr)   nullptr_t;

	template<int SizeT>
	struct pointer_types
	{
		typedef int64_t ptrdiff_t;
		typedef int64_t intptr_t;
		typedef uint64_t uintptr_t;
	};

	template<>
	struct pointer_types<4>
	{
		typedef int32_t ptrdiff_t;
		typedef int32_t intptr_t;
		typedef uint32_t uintptr_t;
	};

	template<>
	struct pointer_types<2>
	{
		typedef int16_t ptrdiff_t;
		typedef int16_t intptr_t;
		typedef uint16_t uintptr_t;
	};

	typedef decltype(sizeof(1)) size_t;
	typedef pointer_types<sizeof(size_t)>::ptrdiff_t ptrdiff_t;
	typedef pointer_types<sizeof(size_t)>::intptr_t intptr_t;
	typedef pointer_types<sizeof(size_t)>::uintptr_t uintptr_t;

	static_assert(sizeof(size_t) == sizeof(ptrdiff_t), "Incorrect size for ptrdiff_t");
	static_assert(sizeof(size_t) == sizeof(intptr_t), "Incorrect size for intptr_t");
	static_assert(sizeof(size_t) == sizeof(uintptr_t), "Incorrect size for uintptr_t");

	static_assert(sizeof(int8_t)   == 1, "Incorrect size for int8_t");
	static_assert(sizeof(uint8_t)  == 1, "Incorrect size for uint8_t");
	static_assert(sizeof(int16_t)  == 2, "Incorrect size for int16_t");
	static_assert(sizeof(uint16_t) == 2, "Incorrect size for uint16_t");
	static_assert(sizeof(int32_t)  == 4, "Incorrect size for int32_t");
	static_assert(sizeof(uint32_t) == 4, "Incorrect size for uint32_t");
	static_assert(sizeof(int64_t)  == 8, "Incorrect size for int64_t");
	static_assert(sizeof(uint64_t) == 8, "Incorrect size for uint64_t");

	static_assert(crstl_is_signed(int8_t), "Incorrect signedness for int8_t");
	static_assert(crstl_is_unsigned(uint8_t), "Incorrect signedness for uint8_t");
	static_assert(crstl_is_signed(int16_t), "Incorrect signedness for int16_t");
	static_assert(crstl_is_unsigned(uint16_t), "Incorrect signedness for uint16_t");
	static_assert(crstl_is_signed(int32_t), "Incorrect signedness for int32_t");
	static_assert(crstl_is_unsigned(uint32_t), "Incorrect signedness for uint32_t");
	static_assert(crstl_is_signed(int64_t), "Incorrect signedness for int64_t");
	static_assert(crstl_is_unsigned(uint64_t), "Incorrect signedness for uint64_t");
};

#if !defined(CRSTL_CHAR8_TYPE)

typedef unsigned char char8_t;

#endif