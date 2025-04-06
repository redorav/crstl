#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

// These headers are so basic that essentially every C++ header ends up including them in one form or the other
// We can make our own and they can work but it can cause conflicts in certain scenarios and it's probably better
// for each platform to take care of them
#if defined(CRSTL_MODULE_DECLARATION)
import <stdint.h>;
import <stddef.h>;
#else
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

crstl_warning_char8_t_keyword_begin
typedef unsigned char char8_t;
crstl_warning_char8_t_keyword_end

#endif