#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <cstdint>;
import <cstddef>;
#elif defined(CRSTL_INITIALIZER_LISTS)
#include <cstdint>
#include <cstddef>
#endif

#if !defined(CRSTL_CHAR8_TYPE)

typedef unsigned char char8_t;

#endif