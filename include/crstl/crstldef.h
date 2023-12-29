#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <stdint.h>;
import <stddef.h>;
#elif defined(CRSTL_INITIALIZER_LISTS)
#include <stdint.h>
#include <stddef.h>
#endif

// Helps with some compilers that don't define e.g. nullptr_t in the global namespace. For classes inside the crstl
// namespace, these declarations help out and they aren't a big issue
namespace crstl
{
	typedef decltype(nullptr) nullptr_t;
	typedef decltype(sizeof(1)) size_t;
};

#if !defined(CRSTL_CHAR8_TYPE)

typedef unsigned char char8_t;

#endif