#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/crstldef.h"

#if !defined(CRSTL_COMPILER_MSVC) && !defined(CRSTL_COMPILER_GCC)
	#define CRSTL_BUILTIN_WCSLEN
#endif

#if !defined(CRSTL_COMPILER_MSVC)
	#define CRSTL_BUILTIN_STRLEN
#endif

extern "C"
{
#if !defined(CRSTL_BUILTIN_STRLEN)
	size_t strlen(const char* str);
#endif

#if !defined(CRSTL_BUILTIN_WCSLEN)
	crstl_dllimport size_t wcslen(const wchar_t* str) crstl_linux_wthrow;
#endif
}

crstl_module_export namespace crstl
{
	inline size_t string_length(const char* str)
	{
#if defined(CRSTL_BUILTIN_STRLEN)
		return __builtin_strlen(str);
#else
		return strlen(str);
#endif
	}

	inline size_t string_length(const char* str, size_t max_length)
	{
		size_t length = string_length(str);
		return length < max_length ? length : max_length;
	}

	inline size_t string_length(const wchar_t* str)
	{
#if defined(CRSTL_BUILTIN_WCSLEN)
		return __builtin_wcslen(str);
#else
		return wcslen(str);
#endif
	}

	inline size_t string_length(const wchar_t* str, size_t max_length)
	{
		size_t length = string_length(str);
		return length < max_length ? length : max_length;
	}

	inline size_t string_length(const char8_t* str)
	{
		return string_length((const char*)str);
	}

	inline size_t string_length(const char8_t* str, size_t max_length)
	{
		size_t length = string_length((const char*)str);
		return length < max_length ? length : max_length;
	}
};
