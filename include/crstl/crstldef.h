#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <stdint.h>;
#else
#include <stdint.h>
#endif

// Helps with some compilers that don't define e.g. nullptr_t in the global namespace. For classes inside the crstl
// namespace, these declarations help out and they aren't a big issue
namespace crstl
{
	typedef decltype(nullptr) nullptr_t;
	typedef decltype(sizeof(1)) size_t;

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
};

#if !defined(CRSTL_CHAR8_TYPE)

typedef unsigned char char8_t;

#endif