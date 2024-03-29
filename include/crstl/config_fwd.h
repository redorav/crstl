#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

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

// Create our own placement new to avoid including header <new> which is quite heavy
// We pass in a dummy parameter to disambiguate from the global placement new. The
// macro is there just for convenience
namespace crstl
{
	enum placement_new
	{
		placement_new_dummy
	};
};

crstl_nodiscard crstl_forceinline void* operator new (crstl::size_t, void* ptr, crstl::placement_new) crstl_noexcept { return ptr; }
crstl_forceinline void operator delete (void*, void*, crstl::placement_new) crstl_noexcept {}

#define crstl_placement_new(x) ::new(x, crstl::placement_new_dummy)