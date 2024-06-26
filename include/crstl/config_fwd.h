#pragma once

#include "crstl/config.h"

// Function forward declarations. This is to avoid includes as one of our priorities is to make compile times
// very fast. Here we can do compiler checks, mangling, whatever we need to conform to the target compiler
// Some functions are well defined across compilers, whereas some are poorly defined which is unfortunate

#if defined(CRSTL_COMPILER_MSVC)
	#define crstl_dllimport __declspec(dllimport)

	#if CRSTL_MSVC_VERSION <= CRSTL_MSVC_2015
	#define crstl_2015_dllimport __declspec(dllimport)
	#endif
#else
	#define crstl_dllimport
#endif

#if !defined(crstl_dllimport)
	#define crstl_dllimport
#endif

#if !defined(crstl_2015_dllimport)
	#define crstl_2015_dllimport
#endif

#if defined(CRSTL_OS_LINUX)
#define crstl_linux_wthrow throw()
#else
#define crstl_linux_wthrow
#endif