#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

// Function forward declarations. This is to avoid includes as one of our priorities is to make compile times
// very fast. Here we can do compiler checks, mangling, whatever we need to conform to the target compiler
// Some functions are well defined across compilers, whereas some are poorly defined which is unfortunate

#if defined(CRSTL_MSVC)
	#define crstl_dllimport __declspec(dllimport)

	#if CRSTL_MSVC_VERSION <= CRSTL_MSVC_2015
	#define crstl_1600_dllimport __declspec(dllimport)
	#endif
#else
	#define crstl_dllimport
#endif

#if !defined(crstl_dllimport)
	#define crstl_dllimport
#endif

#if !defined(crstl_1600_dllimport)
	#define crstl_1600_dllimport
#endif

#if defined(CRSTL_LINUX)
#define crstl_wthrow throw()
#else
#define crstl_wthrow
#endif

extern "C"
{
	crstl::size_t strlen(const char* str);

	void* memset(void* dst, int val, crstl::size_t size);
	void* memcpy(void* destination, void const* source, crstl::size_t size);

	crstl_dllimport crstl::size_t wcslen(const wchar_t* str) crstl_wthrow;

	wchar_t* wmemset(wchar_t* ptr, wchar_t wc, crstl::size_t num) crstl_wthrow;

	crstl_1600_dllimport void* memmove(void* destination, const void* source, crstl::size_t num);
}

extern "C++"
{
	// Forward declare placement new
	crstl_nodiscard void* operator new  (crstl::size_t count, void* ptr) crstl_noexcept;
	crstl_nodiscard void* operator new[](crstl::size_t count, void* ptr) crstl_noexcept;
}