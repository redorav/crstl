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

namespace crstl
{
	typedef decltype(nullptr) nullptr_t;
	typedef decltype(sizeof(1)) size_t;
};

#if !defined(CRSTL_CHAR8_TYPE)

typedef unsigned char char8_t;

#endif