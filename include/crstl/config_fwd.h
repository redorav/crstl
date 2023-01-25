#pragma once

#include "config.h"

#include "crstldef.h"

// Function forward declarations. This is to avoid includes as one of our priorities is to make compile times
// very fast. Here we can do compiler checks, mangling, whatever we need to conform to the target compiler
// Some functions are well defined across compilers, whereas some are poorly defined such as memchr and wmemchr
// which is unfortunate. We'll provide our own implementations of these instead

extern "C"
{
	crstl::size_t strlen(const char* str);
	void* memset(void* dst, int val, crstl::size_t size);
	void* memcpy(void* destination, void const* source, crstl::size_t size);

#if defined(_MSC_VER)
	__declspec(dllimport)
#endif
	crstl::size_t wcslen(const wchar_t* str);
	wchar_t* wmemset(wchar_t* ptr, wchar_t wc, crstl::size_t num);
}

extern "C"
{
#if defined(_MSC_VER) && _MSC_VER <= 1600
	__declspec(dllimport)
#endif
	void* memmove(void* destination, const void* source, crstl::size_t num);
}