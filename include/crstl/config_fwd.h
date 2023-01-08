#pragma once

#include "config.h"

// Function forward declarations. This is to avoid includes as one of our priorities is to make compile times
// very fast. Here we can do compiler checks, mangling, whatever we need to conform to the target compiler

extern "C"
{
	// <string.h

	size_t strlen(const char* str);

	__declspec(dllimport) size_t wcslen(const wchar_t* str);

	void* memset(void* dst, int val, size_t size);

	const void* memchr(const void* ptr, int value, size_t num);

	void* memcpy(void* destination, void const* source, size_t size);

	void* memmove(void* destination, const void* source, size_t num);

	// <wchar.h>

	wchar_t* wmemset(wchar_t* ptr, wchar_t wc, size_t num);

	const wchar_t* wmemchr(const wchar_t* ptr, wchar_t wc, size_t num);
}
