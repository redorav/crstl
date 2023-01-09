#pragma once

#include "config.h"

#include "config_fwd.h"

#include <stdint.h>

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

	template<typename T, int NumElements>
	class basic_fixed_string
	{
	public:

		typedef T                                  value_type;
		typedef T&                                 reference;
		typedef const T&                           const_reference;
		typedef T*                                 pointer;
		typedef const T*                           const_pointer;
		typedef T*                                 iterator;
		typedef const T*                           const_iterator;
		
		enum
		{
			kNumElementsWithZero = NumElements,     // Number of elements, including terminating zero
			kNumElements         = NumElements - 1, // Number of usable characters
			kCharSize            = sizeof(T)
		};

		static const crstl_constexpr size_t npos = (size_t)-1;

		basic_fixed_string() crstl_noexcept : m_length(0)
		{
			m_data[0] = '\0';
		}

		basic_fixed_string(const_pointer string) crstl_noexcept
		{
			assign(string);
		}

		basic_fixed_string(const_pointer string, size_t length) crstl_noexcept
		{
			assign(string, length);
		}

		basic_fixed_string(const_pointer begin, const_pointer end) crstl_noexcept
		{
			assign(begin, end);
		}

		template<int NumElementsOther>
		basic_fixed_string(const basic_fixed_string<T, NumElementsOther>& string) crstl_noexcept
		{
			assign(string);
		}

		basic_fixed_string(const basic_fixed_string& string) crstl_noexcept
		{
			assign(string);
		}

		template<int NumElementsOther>
		basic_fixed_string(const basic_fixed_string<T, NumElements>& string1, const basic_fixed_string<T, NumElementsOther>& string2) crstl_noexcept
		{
			assign(string1);
			append(string2);
		}

		basic_fixed_string(const basic_fixed_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			assign(string, subpos, sublen);
		}

		crstl_constexpr T& at(size_t i) crstl_noexcept
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr const T& at(size_t i) const crstl_noexcept
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		//-------
		// append
		//-------

		// Append a const char* string with a provided length
		crstl_constexpr basic_fixed_string& append(const_pointer string, size_t length) crstl_noexcept
		{
			if (length)
			{
				crstl_assert(m_length + length < kNumElementsWithZero);
				memcpy(m_data + m_length, string, length * kCharSize);
				m_length += (uint32_t)length;
				m_data[m_length] = '\0';
			}
			return *this;
		}

		// Append a const char* string
		crstl_constexpr basic_fixed_string& append(const_pointer string) crstl_noexcept
		{
			size_t length = string_length(string);
			append(string, length);
			return *this;
		}

		// Append a const char* string defined by a begin and end
		crstl_constexpr basic_fixed_string& append(const_pointer begin, const_pointer end) crstl_noexcept
		{
			size_t length = (size_t)(end - begin);
			append(begin, length);
			return *this;
		}

		template<size_t NumElementsOther>
		crstl_constexpr basic_fixed_string& append(const basic_fixed_string<T, NumElementsOther>& string) crstl_noexcept
		{
			append(string.m_data, string.m_length);
			return *this;
		}

		// Append a fixed string
		crstl_constexpr basic_fixed_string& append(const basic_fixed_string& string) crstl_noexcept
		{
			append(string.m_data, string.m_length);
			return *this;
		}

		// Append a substring of a fixed string
		crstl_constexpr basic_fixed_string& append(const basic_fixed_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			crstl_assert(string.length() + subpos + sublen < basic_fixed_string::kNumElementsWithZero);
			append(string.m_data + subpos, sublen);
			return *this;
		}

		// Append n copies of character c
		crstl_constexpr basic_fixed_string& append(size_t n, value_type c) crstl_noexcept
		{
			crstl_assert(m_length + n < kNumElementsWithZero);
			crstl::fill_char(&m_data[m_length], n, c);
			return *this;
		}

		//-------
		// assign
		//-------

		crstl_constexpr basic_fixed_string& assign(const_pointer string, size_t length) crstl_noexcept
		{
			m_length = 0;
			append(string, length);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(const_pointer string) crstl_noexcept
		{
			m_length = 0;
			append(string);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(const_pointer begin, const_pointer end) crstl_noexcept
		{
			m_length = 0;
			append(begin, end);
			return *this;
		}

		template<size_t NumElementsOther>
		crstl_constexpr basic_fixed_string& assign(const basic_fixed_string<T, NumElementsOther>& string) crstl_noexcept
		{
			m_length = 0;
			append(string);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(const basic_fixed_string& string) crstl_noexcept
		{
			m_length = 0;
			append(string);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(const basic_fixed_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			m_length = 0;
			append(string, subpos, sublen);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(size_t n, value_type c) crstl_noexcept
		{
			m_length = 0;
			append(n, c);
			return *this;
		}

		crstl_constexpr T& back() crstl_noexcept
		{
			crstl_assert(m_length > 0);
			return m_data[m_length - 1];
		}

		crstl_constexpr const T& back() const crstl_noexcept
		{
			crstl_assert(m_length > 0);
			return m_data[m_length - 1];
		}

		crstl_constexpr iterator begin() crstl_noexcept { return &m_data[0]; }
		crstl_constexpr const_iterator begin() const crstl_noexcept { return &m_data[0]; }

		crstl_constexpr pointer c_str() crstl_noexcept
		{
			return m_data;
		}

		crstl_constexpr const_pointer c_str() const crstl_noexcept
		{
			return m_data;
		}

		// Returns the maximum size of this container, in bytes
		crstl_constexpr size_t capacity() const crstl_noexcept
		{
			return kNumElementsWithZero * kCharSize;
		}

		crstl_constexpr const_iterator cbegin() const crstl_noexcept { return &m_data[0]; }
		crstl_constexpr const_iterator cend() const crstl_noexcept { return &m_data[0] + m_length; }

		crstl_constexpr void clear() crstl_noexcept
		{
			m_length = 0;
		}

		//--------
		// compare
		//--------

		crstl_constexpr int compare(const T* string) const crstl_noexcept
		{
			return crstl::string_compare(m_data, m_length, string, string_length(string));
		}

		crstl_constexpr int compare(size_t pos, size_t length, const T* string) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_compare(m_data + pos, clamp_length(pos, length), string, string_length(string));
		}

		crstl_constexpr int compare(size_t pos, size_t length, const T* string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_compare(m_data + pos, clamp_length(pos, length), string + subpos, crstl::string_clamp_length(string_length(string), subpos, sublen));
		}

		crstl_constexpr int compare(const basic_fixed_string& string) const crstl_noexcept
		{
			return crstl::string_compare(m_data, m_length, string.m_data, string.m_length);
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_fixed_string& string) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_compare(m_data + pos, clamp_length(pos, length), string.m_data, string.m_length);
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_fixed_string& string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_compare(m_data + pos, clamp_length(pos, length), string.m_data + subpos, string.clamp_length(subpos, sublen));
		}

		crstl_constexpr pointer data() crstl_noexcept { return c_str(); }
		crstl_constexpr const_pointer data() const crstl_noexcept { return c_str(); }

		crstl_constexpr bool empty() const crstl_noexcept { return m_length == 0; }

		crstl_constexpr iterator end() crstl_noexcept { return &m_data[0] + m_length; }
		crstl_constexpr const_iterator end() const crstl_noexcept { return &m_data[0] + m_length; }

		//-----
		// find
		//-----

		// Find a character
		size_t find(value_type c, size_t pos = 0) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			const_pointer ptr = (const_pointer)string_find_char(m_data + pos, c, m_length - pos);
			return ptr ? (size_t)(ptr - m_data) : npos;
		}

		// Find a const char* string with an offset and a length
		size_t find(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			if (needle_string == nullptr || needle_length > m_length || pos > (m_length - needle_length))
			{
				return npos;
			}

			// If we have an empty string, return the offset
			if (needle_length == 0)
			{
				return pos;
			}

			// No point searching if length of needle is longer than the final characters of the string
			const_pointer search_end = m_data + (m_length - needle_length) + 1;
			const_pointer string_end = m_data + m_length;
			const_pointer search_start = m_data + pos;

			while(search_start)
			{
				search_start = string_find_char(search_start, *needle_string, (size_t)(search_end - search_start));

				if (!search_start)
				{
					return npos;
				}

				// If we matched the first character
				if (string_compare(search_start, needle_length, needle_string, needle_length) == 0)
				{
					return (size_t)(search_start - m_data);
				}

				++search_start;
			}

			return npos;
		}

		size_t find(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string, pos, string_length(needle_string));
		}

		size_t find(const basic_fixed_string& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string.m_data, pos, needle_string.m_length);
		}

		crstl_constexpr reference front() crstl_noexcept { m_data[0]; }
		crstl_constexpr const_reference front() const crstl_noexcept { m_data[0]; }

		// Returns the length of the string, in terms of number of characters
		crstl_constexpr size_t length() const crstl_noexcept { return m_length; }

		crstl_constexpr size_t max_size() const crstl_noexcept { return kNumElements; }

		crstl_constexpr void pop_back() crstl_noexcept { crstl_assert(m_length > 0); m_length--; }

		crstl_constexpr reference push_back(value_type c) { append(1, c); return back(); }

		//--------
		// replace
		//--------

		basic_fixed_string& replace(size_t pos, size_t length, const_pointer replace_string, size_t replace_length)
		{
			replace_common(pos, length, replace_length);
			memcpy(m_data + pos, replace_string, replace_length * kCharSize);
			return *this;
		}

		basic_fixed_string& replace(size_t pos, size_t length, size_t n, value_type c)
		{
			replace_common(pos, length, n);
			crstl::fill_char(m_data + pos, n, c);
			return *this;
		}

		crstl_constexpr basic_fixed_string& replace(size_t pos, size_t length, const_pointer replace_string) crstl_noexcept
		{
			return replace(pos, length, replace_string, crstl::string_length(replace_string));
		}

		crstl_constexpr basic_fixed_string& replace(size_t pos, size_t length, const basic_fixed_string& replace_string) crstl_noexcept
		{
			return replace(pos, length, replace_string.m_data, replace_string.m_length);
		}

		crstl_constexpr basic_fixed_string& replace(size_t pos, size_t length, const basic_fixed_string& replace_string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			sublen = sublen < replace_string.m_length ? sublen : replace_string.m_length;
			return replace(pos, length, replace_string.m_data + subpos, sublen);
		}

		crstl_constexpr basic_fixed_string& replace(const_pointer begin, const_pointer end, const_pointer replace_string) crstl_noexcept
		{
			return replace(begin, (size_t)(end - begin), replace_string, crstl::string_length(replace_string));
		}

		crstl_constexpr basic_fixed_string& replace(const_pointer begin, const_pointer end, const_pointer replace_string, size_t replace_length) crstl_noexcept
		{
			return replace(begin, (size_t)(end - begin), replace_string, replace_length);
		}

		crstl_constexpr basic_fixed_string& replace(const_pointer begin, const_pointer end, const basic_fixed_string& replace_string) crstl_noexcept
		{
			return replace(begin, (size_t)(end - begin), replace_string.m_data, replace_string.m_length);
		}

		//-----
		// rfind
		//-----

		size_t rfind(value_type c, size_t pos = npos) const crstl_noexcept
		{
			pos = pos < m_length ? pos : m_length;
			const_pointer ptr = (const_pointer)string_rfind_char(m_data + pos, c, m_length - pos);
			return ptr ? (size_t)(ptr - m_data) : npos;
		}

		// Find a const char* string with an offset and a length
		size_t rfind(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			if (needle_string == nullptr || needle_length > m_length || pos < needle_length)
			{
				return npos;
			}

			// If we have an empty string, return the offset
			if (needle_length == 0)
			{
				return pos;
			}

			pos = pos < m_length ? pos : m_length;

			// No point searching if length of needle is longer than the final characters of the string
			const_pointer search_end = m_data + (m_length - needle_length) + 1;
			const_pointer string_end = m_data + m_length;
			const_pointer search_start = m_data + pos;

			while(search_start != m_data)
			{
				search_start = string_rfind_char(search_start, *needle_string, (size_t)(search_end - search_start));

				if (!search_start)
				{
					return npos;
				}

				// If we matched the first character
				if (string_compare(search_start, needle_length, needle_string, needle_length) == 0)
				{
					return (size_t)(search_start - m_data);
				}

				search_start--;
			}

			return npos;
		}

		size_t rfind(const_pointer needle_string, size_t pos = npos) const crstl_noexcept
		{
			return rfind(needle_string, pos, string_length(needle_string));
		}

		size_t rfind(const basic_fixed_string& needle_string, size_t pos = npos) const crstl_noexcept
		{
			return rfind(needle_string.m_data, pos, needle_string.m_length);
		}

		crstl_constexpr size_t size() const crstl_noexcept { return length(); }

		crstl_constexpr basic_fixed_string substr(size_t pos, size_t length = npos) const crstl_noexcept
		{
			return basic_fixed_string(*this, pos, length);
		}

		//----------
		// Operators
		//----------

		crstl_constexpr T& operator [](size_t i)
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr const T& operator [](size_t i) const crstl_noexcept
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr basic_fixed_string& operator += (const basic_fixed_string& string) crstl_noexcept
		{
			append(string);
			return *this;
		}

		crstl_constexpr basic_fixed_string& operator += (const_pointer string) crstl_noexcept
		{
			append(string);
			return *this;
		}

		crstl_constexpr bool operator == (const_pointer string) crstl_noexcept
		{
			return compare(string) == 0;
		}

		crstl_constexpr bool operator != (const_pointer string) crstl_noexcept
		{
			return compare(string) != 0;
		}

		crstl_constexpr bool operator == (const basic_fixed_string& string) crstl_noexcept
		{
			return compare(string) == 0;
		}

		crstl_constexpr bool operator != (const basic_fixed_string& string) crstl_noexcept
		{
			return compare(string) != 0;
		}

		T m_data[NumElements];

		uint32_t m_length;

	private:

		crstl_constexpr void replace_common(size_t pos, size_t length, size_t replace_length)
		{
			crstl_assert(pos < m_length);
			crstl_assert(length <= m_length);

			size_t replace_difference = (replace_length - length);

			if (replace_length > length)
			{
				crstl_assert(m_length + replace_difference < kNumElementsWithZero);
			}

			// Move the parts that would be stomped or leave gaps, including the null terminator
			if (replace_difference != 0)
			{
				memmove(m_data + pos + replace_length, m_data + pos + length, (m_length - (pos + length) + 1) * kCharSize);
			}

			// This happens before the actual writing of data so take care not to use m_length after using this function
			m_length = (uint32_t)(m_length - length + replace_length);
		}

		// Given a position and a length, return the length that fits the string
		crstl_constexpr size_t clamp_length(size_t pos, size_t length) const crstl_noexcept
		{
			return crstl::string_clamp_length(m_length, pos, length);
		}
	};

	// Return the concatenation of two equally-sized strings
	template<typename T, int NumElements>
	basic_fixed_string<T, NumElements> operator + (const basic_fixed_string<T, NumElements>& string1, const basic_fixed_string<T, NumElements>& string2)
	{
		return basic_fixed_string<T, NumElements>(string1, string2);
	}

	// Return the concatenation of differently-sized strings. As we have to choose a size for the return string, we'll select the largest one,
	// on the assumption that it is the most likely to hold the sum of the two values
	template<typename T, int NumElements, int NumElementsOther>
	basic_fixed_string<T, (NumElements > NumElementsOther ? NumElements : NumElementsOther)> operator + (const basic_fixed_string<T, NumElements>& string1, const basic_fixed_string<T, NumElementsOther>& string2)
	{
		return basic_fixed_string<T, (NumElements > NumElementsOther ? NumElements : NumElementsOther)>(string1, string2);
	}
}