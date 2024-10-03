#pragma once

#include "crstl/config.h"

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