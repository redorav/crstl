#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/crstldef.h"

// This include is very cheap in compile times and hard
// to get right outside of its actual implementation
#if defined(CRSTL_MODULE_DECLARATION)
import <stdarg.h>;
#else
#include <stdarg.h>
#endif

extern "C"
{
	crstl_2015_dllimport int vsnprintf(char* s, crstl::size_t n, const char* format, va_list arg) crstl_linux_wthrow;

	crstl_dllimport int tolower(int c) crstl_linux_wthrow;
	crstl_dllimport int toupper(int c) crstl_linux_wthrow;
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

	inline size_t string_length(const char* str) { return strlen(str); }
	inline size_t string_length(const char* str, size_t max_length)
	{
		size_t length = strlen(str);
		return length < max_length ? length : max_length;
	}

	inline size_t string_length(const wchar_t* str) { return wcslen(str); }
	inline size_t string_length(const wchar_t* str, size_t max_length)
	{
		size_t length = wcslen(str);
		return length < max_length ? length : max_length;
	}

	inline size_t string_length(const char8_t* str) { return strlen((const char*)str); }
	inline size_t string_length(const char8_t* str, size_t max_length)
	{
		size_t length = strlen((const char*)str);
		return length < max_length ? length : max_length;
	}

	// = 0   They compare equal
	// < 0   Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter.
	// > 0   Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer.
	template<typename T>
	inline crstl_constexpr int string_compare(const T* string1, size_t length1, const T* string2, size_t length2)
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

	template<typename T>
	inline crstl_constexpr int string_comparei(const T* string1, size_t length1, const T* string2, size_t length2)
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

	template<typename T>
	const T* string_find_of(const T* string, size_t n, const T* needles, size_t needle_length)
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
	const T* string_rfind_of(const T* string, size_t n, const T* needles, size_t needle_length)
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

	//----------------------------------------------------------------------------------------------------------
	// UTF Utilities
	// Code mainly based on https://github.com/Davipb/utf8-utf16-converter/blob/master/converter/src/converter.c
	//----------------------------------------------------------------------------------------------------------

	// Every character can be represented as a 32-bit uint
	typedef uint32_t codepoint_t;

	typedef uint8_t utf8_t;

	typedef uint16_t utf16_t;

	// The basic multilingual plane is equivalent to UCS2
	const codepoint_t BasicMultilingualPlaneEnd = 0xffff;

	// Maximum Unicode codepoint (21 bits) although not all are valid
	const codepoint_t UnicodeMax = 0x10ffff;

	// Unicode value to use as 'invalid'
	const codepoint_t UnicodeInvalid = 0xfffd;

	// If a masked character equals this value, it is a surrogate and needs further decoding
	const uint32_t GenericSurrogateValue = 0xd800;

	// Mask to check for surrogates
	const uint32_t GenericSurrogateMask = 0xf800;

	// If character masked with surrogate mask equals this value, it is a high surrogate
	const uint32_t HighSurrogateValue = 0xd800;

	// If character masked with surrogate mask equals this value, it is a low surrogate
	const uint32_t LowSurrogateValue = 0xdc00;

	// Mask to check for low or high surrogates
	const uint32_t SurrogateMask = 0xfc00;

	// The value subtracted from a codepoint before encoding it in a surrogate pair
	const uint32_t SurrogateCodepointOffset = 0x10000;

	// A mask applied to a surrogate to extract the codepoint value
	const uint32_t SurrogateCodepointMask = 0x03FF;

	// Number of bits of SurrogateCodepointMask
	const uint32_t SurrogateCodepointBits = 10;

	// Maximum value that can be encoded in utf-8 given a number of bits
	const uint32_t UTF8_1_Max = 0x7f;

	const uint32_t UTF8_2_Max = 0x7ff;

	const uint32_t UTF8_3_Max = 0xffff;

	const uint32_t UTF8_4_Max = 0x10ffff;

	// Marks a continuation byte
	const uint32_t ContinuationValue = 0x80;

	// Mask incoming byte with this mask to check for a continuation byte
	const uint32_t ContinuationMask = 0xc0;

	// Number of bits contained in a utf-8 continuation byte
	const uint32_t ContinuationCodepointBits = 6;

	struct utf8_pattern
	{
		// The mask that should be applied to the character before testing it
		utf8_t mask;

		// The value that the character should be tested against after applying the mask
		utf8_t value;
	};

	// The patterns for leading bytes of a UTF-8 codepoint encoding
	// Each pattern represents the leading byte for a character encoded with N UTF-8 bytes,
	// where N is the index + 1
	const utf8_pattern utf8_leading_patterns[] =
	{
		{ 0x80, 0x00 }, // 0xxxxxxx
		{ 0xE0, 0xC0 }, // 110xxxxx
		{ 0xF0, 0xE0 }, // 1110xxxx
		{ 0xF8, 0xF0 }  // 11110xxx
	};

	inline size_t utf8_bytes(codepoint_t codepoint)
	{
		if (codepoint <= UTF8_1_Max)
			return 1;

		if (codepoint <= UTF8_2_Max)
			return 2;

		if (codepoint <= UTF8_3_Max)
			return 3;

		return 4;
	}

	namespace utf_result
	{
		enum t : size_t
		{
			success = 0,
			invalid = (size_t)-1,  // Invalid decoding encountered
			no_memory = (size_t)-2 // Not enough memory to decode
		};
	};

	// utf8: The utf-8 string
	// length: The length of the utf-8 string, in bytes
	// offset: An offset into the utf-8 stream
	inline codepoint_t decode_utf8(const utf8_t* utf8, size_t length, size_t& offset)
	{
		utf8_t leading = utf8[offset];

		// The number of bytes that are used to encode the codepoint
		size_t encoding_length = 0;

		// The pattern of the leading byte
		utf8_pattern leading_pattern = {};

		// If the leading byte matches the current leading pattern
		bool matches = false;

		// Try to search for a matching leading pattern
		for (size_t i = 0; i < 4; ++i)
		{
			leading_pattern = utf8_leading_patterns[i];
			matches = (leading & leading_pattern.mask) == leading_pattern.value;

			if (matches)
			{
				encoding_length = i + 1;
				break;
			}
		}

		// If there's no there's not enough data
		if (!matches || offset + encoding_length > length)
		{
			return crstl::UnicodeInvalid;
		}

		offset++;

		// Remove the pattern mask to extract the codepoint
		codepoint_t codepoint = leading & ~leading_pattern.mask;

		// Loop through continuation bytes to complete the codepoint
		for (size_t i = 1; i < encoding_length; ++i)
		{
			utf8_t continuation = utf8[offset];

			if ((continuation & ContinuationMask) != ContinuationValue)
			{
				return crstl::UnicodeInvalid;
			}

			codepoint <<= ContinuationCodepointBits;
			codepoint |= continuation & ~ContinuationMask;

			offset++;
		}

		size_t size_bytes = utf8_bytes(codepoint);

		// Incorrect length used 
		if(size_bytes != encoding_length || codepoint > UnicodeMax)
		{
			return crstl::UnicodeInvalid;
		}

		// Surrogates are invalid Unicode codepoints, and should only be used in UTF-16
		if (codepoint < BasicMultilingualPlaneEnd && ((codepoint & GenericSurrogateMask) == GenericSurrogateValue))
		{
			return crstl::UnicodeInvalid;
		}

		return codepoint;
	}

	inline codepoint_t decode_utf16(const utf16_t* utf16, size_t length, size_t& offset)
	{
		utf16_t high = utf16[offset];

		offset++;

		// Basic Multilingual Plane character
		if ((high & GenericSurrogateMask) != GenericSurrogateValue)
		{
			return high;
		}

		// Unmatched low surrogate, invalid
		if ((high & SurrogateMask) != HighSurrogateValue)
		{
			return crstl::UnicodeInvalid;
		}

		// String ended with an unmatched high surrogate, invalid
		if (offset >= length)
		{
			return crstl::UnicodeInvalid;
		}

		utf16_t low = utf16[offset];

		// Unmatched high surrogate, invalid
		if ((low & SurrogateMask) != LowSurrogateValue)
		{
			return crstl::UnicodeInvalid;
		}

		// Two correctly matched surrogates, increase index to indicate we've consumed two characters
		offset++;

		// The high bits of the codepoint are the value bits of the high surrogate
		// The low bits of the codepoint are the value bits of the low surrogate
		codepoint_t result = high & SurrogateCodepointMask;
		result <<= SurrogateCodepointBits;
		result |= low & SurrogateCodepointMask;
		result += SurrogateCodepointOffset;

		return result;
	}

	inline size_t encode_utf8(codepoint_t codepoint, utf8_t* utf8, size_t length, size_t offset)
	{
		size_t size_bytes = utf8_bytes(codepoint);

		// Not enough space
		if (offset + size_bytes > length)
		{
			return 0;
		}

		// Write the continuation bytes in reverse order first
		for (size_t i = size_bytes - 1; i > 0; i--)
		{
			utf8_t cont = (utf8_t)(codepoint & ~ContinuationMask);
			cont |= ContinuationValue;

			utf8[offset + i] = cont;
			codepoint >>= ContinuationCodepointBits;
		}

		// Write the leading byte
		utf8_pattern pattern = utf8_leading_patterns[size_bytes - 1];

		utf8_t lead = codepoint & ~(pattern.mask);
		lead |= pattern.value;

		utf8[offset] = lead;

		return size_bytes;
	}

	inline size_t encode_utf16(codepoint_t codepoint, utf16_t* utf16, size_t length, size_t offset)
	{
		// Not enough space
		if (offset >= length)
		{
			return 0;
		}

		if (codepoint <= BasicMultilingualPlaneEnd)
		{
			utf16[offset] = (utf16_t)codepoint;
			return 1;
		}

		// Not enough space on the string for two surrogates
		if (offset + 1 >= length)
		{
			return 0;
		}

		codepoint -= SurrogateCodepointOffset;

		utf16_t low = LowSurrogateValue;
		low |= codepoint & SurrogateCodepointMask;

		codepoint >>= ContinuationCodepointBits;

		utf16_t high = HighSurrogateValue;
		high |= codepoint & SurrogateCodepointMask;

		utf16[offset] = high;
		utf16[offset + 1] = low;

		return 2;
	}

	// All possible combinations of decoding can be handled here
	// We assume that char/char8_t is utf-8, char16_t is utf-16 and char32_t is utf-32
	// wchar_t is a special case as it is assumed to be utf16 on Windows and utf-32 on
	// anything else

	inline utf_result::t decode_chunk(char* dst_start, const char* dst_end, const wchar_t* src_start, const wchar_t* src_end, size_t& utf8_offset, size_t& utf16_offset)
	{
		crstl_assert(src_end >= src_start);
		crstl_assert(dst_end >= dst_start);

		size_t src_length = (size_t)(src_end - src_start);
		size_t dst_length = (size_t)(dst_end - dst_start);

		utf16_offset = 0;
		utf8_offset = 0;

		while (utf8_offset < dst_length && utf16_offset < src_length)
		{
			size_t utf16_length = 0;
			codepoint_t cp = decode_utf16((const utf16_t*)(src_start + utf16_offset), src_length, utf16_length);

			if (cp == crstl::UnicodeInvalid)
			{
				return utf_result::invalid;
			}

			size_t utf8_length = encode_utf8(cp, (utf8_t*)dst_start, dst_length, utf8_offset);

			if (utf8_length == 0)
			{
				return utf_result::no_memory;
			}

			utf16_offset += utf16_length;
			utf8_offset += utf8_length;
		}

		// Return whether we processed the entire utf-16 string
		if (utf16_offset == src_length)
		{
			return utf_result::success;
		}
		else
		{
			return utf_result::no_memory;
		}
	}

	inline utf_result::t decode_chunk(wchar_t* dst_start, const wchar_t* dst_end, const char* src_start, const char* src_end, size_t& utf16_offset, size_t& utf8_offset)
	{
		size_t src_length = (size_t)(src_end - src_start);
		size_t dst_length = (size_t)(dst_end - dst_start);

		utf16_offset = 0;
		utf8_offset = 0;

		while (utf16_offset < dst_length && utf8_offset < src_length)
		{
			size_t utf8_length = 0;
			codepoint_t cp = decode_utf8((const utf8_t*)(src_start + utf8_offset), src_length, utf8_length);

			if (cp == crstl::UnicodeInvalid)
			{
				return utf_result::invalid;
			}

			size_t utf16_length = encode_utf16(cp, (utf16_t*)dst_start, dst_length, utf16_offset);

			if (utf16_length == 0)
			{
				return utf_result::no_memory;
			}

			utf8_offset += utf8_length;
			utf16_offset += utf16_length;
		}

		// Return whether we processed the entire utf-8 string
		if (utf8_offset == src_length)
		{
			return utf_result::success;
		}
		else
		{
			return utf_result::no_memory;
		}
	}

	inline utf_result::t decode_chunk(char8_t* dst_start, const char8_t* dst_end, const wchar_t* src_start, const wchar_t* src_end, size_t& utf8_offset, size_t& utf16_offset)
	{
		return decode_chunk((char*)dst_start, (const char*)dst_end, src_start, src_end, utf8_offset, utf16_offset);
	}

	inline utf_result::t decode_chunk(wchar_t* dst_start, const wchar_t* dst_end, const char8_t* src_start, const char8_t* src_end, size_t& utf16_offset, size_t& utf8_offset)
	{
		return decode_chunk(dst_start, dst_end, (const char*)src_start, (const char*)src_end, utf8_offset, utf16_offset);
	}
};
