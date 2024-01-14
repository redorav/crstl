#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/crstldef.h"

#include "crstl/utility/memory_ops.h"

#include "crstl/utility/string_length.h"

// This include is very cheap in compile times and hard
// to get right outside of its actual implementation
#if defined(CRSTL_MODULE_DECLARATION)
import <stdarg.h>;
#else
#include <stdarg.h>
#endif

extern "C"
{
	crstl_2015_dllimport int vsnprintf(char* s, size_t n, const char* format, va_list arg) crstl_linux_wthrow;
}

crstl_module_export namespace crstl
{
	// enable_if for strings
	template<bool Test, typename T = void> struct string_enable_if;
	template<typename T> struct string_enable_if<true, T> { typedef T type; };

	// We use these to disambiguate between string literals, character arrays and char*/const char*
	// This is a runtime performance optimization, as we don't need to call strlen on strings we know
	// the size of at compile time. It's also a security measure, as we can clamp the calculated length
	// of a char array to its maximum declared size, in case a sprintf went wrong
	class char_dummy;
	template<typename T> struct is_char_ptr {};
	template<> struct is_char_ptr<const char*> { typedef char_dummy* type; static const bool value = true; };
	template<> struct is_char_ptr<char*> { typedef char_dummy* type; static const bool value = true; };
	template<> struct is_char_ptr<const wchar_t*> { typedef char_dummy* type; static const bool value = true; };
	template<> struct is_char_ptr<wchar_t*> { typedef char_dummy* type; static const bool value = true; };

	#define crstl_is_char_ptr(T) typename crstl::is_char_ptr<T>::type = nullptr

	#define crstl_return_is_char_ptr(Q, T) typename crstl::string_enable_if<crstl::is_char_ptr<Q>::value, T>::type

	struct ctor_concatenate {};

	struct ctor_no_initialize {};

	// = 0   They compare equal
	// < 0   Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter.
	// > 0   Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer.
	template<typename T>
	inline crstl_constexpr14 int string_compare(const T* string1, size_t length1, const T* string2, size_t length2)
	{
		size_t min_length = length1 < length2 ? length1 : length2;

		while (min_length != 0)
		{
			T char1 = *string1, char2 = *string2;
			if (char1 != char2)
			{
				return char1 < char2 ? -1 : 1;
			}

			string1++;
			string2++;
			min_length--;
		}

		if (length1 == length2)
		{
			return 0;
		}
		else
		{
			return length1 < length2 ? -1 : 1;
		}
	}

	inline int tolower(int ch)
	{
		if (ch >= 'A' && ch <= 'Z')
		{
			return ch | 0x20;
		}
	
		return ch;
	}
	
	inline int toupper(int ch)
	{
		if (ch >= 'a' && ch <= 'z')
		{
			return ch ^ 0x20;
		}
	
		return ch;
	}

	template<typename T>
	inline crstl_constexpr14 int string_comparei(const T* string1, size_t length1, const T* string2, size_t length2)
	{
		if (length1 == length2)
		{
			while (length1 != 0)
			{
				T char1 = (T)tolower(*string1), char2 = (T)tolower(*string2);
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
		else
		{
			return length1 < length2 ? -1 : 1;
		}
	}

	inline size_t string_clamp_length(size_t maxLength, size_t pos, size_t length)
	{
		return length > maxLength - pos ? maxLength - pos : length;
	}

	template<typename T>
	inline const T* string_find_char(const T* string, T c, size_t n)
	{
		for(const T* ptr = string; ptr != string + n; ++ptr)
		{
			if (*ptr == c)
			{
				return ptr;
			}
		}

		return nullptr;
	}

	template<typename T>
	inline const T* string_rfind_char(const T* string, T c, size_t n)
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

	template<typename T>
	inline const T* string_find_of(const T* string, size_t n, const T* needles, size_t needle_length)
	{
		for (const T* ptr = string; ptr != string + n; ++ptr)
		{
			for (const T* ptr2 = needles; ptr2 != needles + needle_length; ++ptr2)
			{
				if (*ptr == *ptr2)
				{
					return ptr;
				}
			}
		}

		return nullptr;
	}

	template<typename T>
	inline const T* string_rfind_of(const T* string, size_t n, const T* needles, size_t needle_length)
	{
		for (const T* ptr = string; ptr != string - n; --ptr)
		{
			for (const T* ptr2 = needles; ptr2 != needles + needle_length; ++ptr2)
			{
				if (*ptr == *ptr2)
				{
					return ptr;
				}
			}
		}

		return nullptr;
	}

	template<typename T>
	inline const T* string_find(const T* string, size_t length, const T* needle_string, size_t needle_length)
	{
		if (needle_string == nullptr || needle_length > length)
		{
			return nullptr;
		}

		// If we're looking for an empty string, return the original string
		if (needle_length == 0)
		{
			return string;
		}

		// No point searching if length of needle is longer than the final characters of the string
		const T* search_start = string;
		const T* search_end = string + (length - needle_length) + 1;

		while (search_start)
		{
			search_start = string_find_char(search_start, *needle_string, (size_t)(search_end - search_start));

			if (!search_start)
			{
				return nullptr;
			}

			// If we matched the first character
			if (string_compare(search_start, needle_length, needle_string, needle_length) == 0)
			{
				return search_start;
			}

			++search_start;
		}

		return nullptr;
	}

	template<typename T>
	inline const T* string_rfind(const T* string, size_t length, const T* needle_string, size_t needle_length)
	{
		if (needle_string == nullptr || needle_length > length)
		{
			return nullptr;
		}

		// If we have an empty string, return the offset
		if (needle_length == 0)
		{
			return string;
		}

		// No point searching if length of needle is longer than the final characters of the string
		const T* search_start = string;
		const T* search_end = string - length;

		while (search_start != search_end)
		{
			search_start = string_rfind_char(search_start, *needle_string, (size_t)(search_start - search_end));

			if (!search_start)
			{
				return nullptr;
			}

			// If we matched the first character
			if (string_compare(search_start, needle_length, needle_string, needle_length) == 0)
			{
				return search_start;
			}

			search_start--;
		}

		return nullptr;
	}

	template<typename T>
	inline T* erase_all(T* string, size_t length, size_t pos, const T needle_char)
	{
		const T* string_end = string + length;
		const T* found_char = crstl::string_find_char(string + pos, needle_char, length - pos);
		T* block_dst = (T*)found_char;

		while (found_char)
		{
			const T* found_char_next = crstl::string_find_char(found_char + 1, needle_char, (size_t)(string_end - (found_char + 1)));
			const T* block_src = found_char + 1;
			const T* block_end = found_char_next ? found_char_next : string_end;

			const size_t block_length = (size_t)(block_end - block_src);

			for (size_t i = 0; i < block_length; ++i)
			{
				block_dst[i] = block_src[i];
			}

			found_char = found_char_next;
			block_dst += block_length;
		}

		return block_dst;
	}

	template<typename T>
	inline T* erase_all(T* string, size_t length, size_t pos, const T* needle_string, size_t needle_length)
	{
		const T* string_end = string + length;
		const T* found_string = crstl::string_find(string + pos, length - pos, needle_string, needle_length);
		T* block_dst = (T*)found_string;

		while (found_string)
		{
			// Try to find the string again. We only copy up to the block where the next string is, or the end of the string
			// We copy in blocks, for instance if we have the following string: "thread needle thread needle" -> becomes "thread  thread "
			// we'll copy the contents up to the next time we find needle, and so on until we reach the end
			const T* found_string_next = crstl::string_find(found_string + 1, (size_t)(string_end - (found_string + 1)), needle_string, needle_length);
			const T* block_src = found_string + needle_length;
			const T* block_end = found_string_next ? found_string_next : string_end;
			const size_t block_length = (size_t)(block_end - block_src);

			for (size_t i = 0; i < block_length; ++i)
			{
				block_dst[i] = block_src[i];
			}

			found_string = found_string_next;
			block_dst += block_length;
		}

		return block_dst;
	}

	inline crstl_constexpr14 void fill_char(char* destination, size_t n, char c)
	{
		if (n)
		{
			memory_set(destination, c, n);
		}
	}

	inline crstl_constexpr14 void fill_char(wchar_t* destination, size_t n, char c)
	{
		if (n)
		{
			wmemory_set(destination, c, n);
		}
	}
};
