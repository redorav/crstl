#pragma once

#include "config.h"

#include "crstldef.h"

// Function forward declarations. This is to avoid includes as one of our priorities is to make compile times
// very fast. Here we can do compiler checks, mangling, whatever we need to conform to the target compiler
// Some functions are well defined across compilers, whereas some are poorly defined such as memchr and wmemchr
// which is unfortunate. We'll provide our own implementations of these instead

#if defined(_MSC_VER)
	#define crstl_dllimport __declspec(dllimport)

	#if _MSC_VER <= 1600
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
	crstl_dllimport crstl::size_t strnlen(const char* str, crstl::size_t max_count);

	void* memset(void* dst, int val, crstl::size_t size);
	void* memcpy(void* destination, void const* source, crstl::size_t size);

	crstl_dllimport crstl::size_t wcslen(const wchar_t* str) crstl_wthrow;
	crstl_dllimport crstl::size_t wcsnlen(const wchar_t* str, crstl::size_t max_count) crstl_wthrow;

	wchar_t* wmemset(wchar_t* ptr, wchar_t wc, crstl::size_t num) crstl_wthrow;

	crstl_1600_dllimport void* memmove(void* destination, const void* source, crstl::size_t num);

	crstl_dllimport int tolower(int c) crstl_wthrow;
	crstl_dllimport int toupper(int c) crstl_wthrow;
}
