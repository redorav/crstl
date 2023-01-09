#pragma once

#include "config.h"

#include "config_fwd.h"

namespace crstl
{
	inline size_t string_length(const char* str)
	{
		return strlen(str);
	}

	inline size_t string_length(const wchar_t* str)
	{
		return wcslen(str);
	}

	// = 0   They compare equal
	// < 0   Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter.
	// > 0   Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer.
	template<typename T>
	inline crstl_constexpr int string_compare(const T* string1, size_t length1, const T* string2, size_t length2)
	{
		if (length1 == length2)
		{
			while (length1 != 0)
			{
				T char1 = *string1, char2 = *string2;
				if (char1 != char2)
				{
					return char1 < char2 ? -1 : 1;
				}

				string1++;
				string2++;
				length1--;
			}

			return 0;
		}
		else if (length1 < length2)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}

	size_t string_clamp_length(size_t maxLength, size_t pos, size_t length)
	{
		return length > maxLength - pos ? maxLength - pos : length;
	}

	inline const char* string_find_char(const char* string, char c, size_t n)
	{
		return (const char*)memchr(string, c, n);
	}

	inline const wchar_t* string_find_char(const wchar_t* string, wchar_t c, size_t n)
	{
		return (const wchar_t*)wmemchr(string, c, n);
	}

	template<typename T>
	const T* string_rfind_char(const T* string, T c, size_t n)
	{
		for (const T* ptr = string; ptr != string - n; --ptr)
		{
			if (*ptr == c)
			{
				return ptr;
			}
		}

		return nullptr;
	}

	inline crstl_constexpr void fill_char(char* destination, size_t n, char c)
	{
		if (n)
		{
			memset(destination, c, n);
		}
	}

	inline crstl_constexpr void fill_char(wchar_t* destination, size_t n, char c)
	{
		if (n)
		{
			wmemset(destination, c, n);
		}
	}
};