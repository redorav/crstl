#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

#include <stdint.h>

namespace crstl
{
	typedef decltype(nullptr) nullptr_t;
};

#if !defined(CRSTL_CHAR8_TYPE)

typedef unsigned char char8_t;

#endif