#pragma once

#include "string_common.h"

#include "crstldef.h"

namespace crstl
{
	template<typename T, int NumElements>
	class basic_fixed_string
	{
	public:

		typedef T        value_type;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T*       iterator;
		typedef const T* const_iterator;
		
		enum
		{
			kCharacterCapacityWithZero = NumElements,     // Number of elements, including terminating zero
			kCharacterCapacity         = NumElements - 1, // Number of usable characters
			kCharSize                  = sizeof(T)
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

		template<int OtherNumElements>
		basic_fixed_string(const basic_fixed_string<T, OtherNumElements>& string) crstl_noexcept
		{
			assign(string);
		}

		basic_fixed_string(const basic_fixed_string& string) crstl_noexcept
		{
			assign(string);
		}

		template<int OtherNumElements>
		basic_fixed_string(const basic_fixed_string<T, NumElements>& string1, const basic_fixed_string<T, OtherNumElements>& string2) crstl_noexcept
		{
			assign(string1);
			append(string2);
		}

		basic_fixed_string(const basic_fixed_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			assign(string, subpos, sublen);
		}

		crstl_constexpr reference at(size_t i) crstl_noexcept
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr const_reference at(size_t i) const crstl_noexcept
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
				crstl_assert(m_length + length < kCharacterCapacityWithZero);
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

		template<int OtherNumElements>
		crstl_constexpr basic_fixed_string& append(const basic_fixed_string<T, OtherNumElements>& string) crstl_noexcept
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
			crstl_assert(string.length() + subpos + sublen < basic_fixed_string::kCharacterCapacityWithZero);
			append(string.m_data + subpos, sublen);
			return *this;
		}

		// Append n copies of character c
		crstl_constexpr basic_fixed_string& append(size_t n, value_type c) crstl_noexcept
		{
			crstl_assert(m_length + n < kCharacterCapacityWithZero);
			crstl::fill_char(&m_data[m_length], n, c);
			return *this;
		}

		//---------------
		// append_convert
		//---------------

		template<typename OtherCharacterType>
		crstl_constexpr basic_fixed_string& append_convert(const OtherCharacterType* string, size_t length) crstl_noexcept
		{
			const OtherCharacterType* stringEnd = string + length;
			value_type* dataStart = m_data + m_length;

			size_t sizeBytes = 0;
			bool success = decode_chunk(dataStart, dataStart + (kCharacterCapacity - m_length), string, stringEnd, sizeBytes);
			crstl_assert(success);

			m_length += (uint32_t)sizeBytes;
			m_data[m_length] = '\0';

			return *this;
		}

		template<typename OtherCharacterType>
		crstl_constexpr basic_fixed_string& append_convert(const OtherCharacterType* string) crstl_noexcept
		{
			return append_convert(string, crstl::string_length(string));
		}

		template<typename OtherCharacterType, int OtherN>
		crstl_constexpr basic_fixed_string& append_convert(const basic_fixed_string<OtherCharacterType, OtherN>& string) crstl_noexcept
		{
			append_convert(string.c_str(), string.length());
			return *this;
		}

		// If we append_convert with our own type, just use append, no need for conversion
		crstl_constexpr basic_fixed_string& append_convert(const_pointer string, size_t length) crstl_noexcept
		{
			append(string, length);
			return *this;
		}

		//---------------
		// append_sprintf
		//---------------

		// Append a const char* string with a provided length
		crstl_constexpr basic_fixed_string& append_sprintf(const_pointer format, ...) crstl_noexcept
		{
			va_list va_arguments;
			va_start(va_arguments, format);

			size_t remaining_length = kCharacterCapacityWithZero - m_length;

			// Try to copy, limiting the number of characters to what we have available
			int char_count = vsnprintf(m_data + m_length, remaining_length, format, va_arguments);

			// It is a formatting error to 
			crstl_assert(char_count >= 0);

			if(char_count >= 0)
			{
				// If we have asserts enabled make sure we notify
				crstl_assert((size_t)char_count < remaining_length);

				size_t copied_length = char_count < remaining_length ? char_count : remaining_length;
				m_length += (uint32_t)copied_length;

				m_data[m_length] = '\0';
			}

			va_end(va_arguments);

			return *this;
		}

		//-------
		// assign
		//-------

		crstl_constexpr basic_fixed_string& assign(const_pointer string, size_t length) crstl_noexcept
		{
			clear();
			append(string, length);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(const_pointer string) crstl_noexcept
		{
			clear();
			append(string);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(const_pointer begin, const_pointer end) crstl_noexcept
		{
			clear();
			append(begin, end);
			return *this;
		}

		template<int OtherNumElements>
		crstl_constexpr basic_fixed_string& assign(const basic_fixed_string<T, OtherNumElements>& string) crstl_noexcept
		{
			clear();
			append(string);
			return *this;
		}

		crstl_constexpr basic_fixed_string& assign(const basic_fixed_string& string) crstl_noexcept
		{
			clear();
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

		//---------------
		// assign_convert
		//---------------

		template<typename OtherCharacterType>
		crstl_constexpr basic_fixed_string& assign_convert(const OtherCharacterType* string, size_t length) crstl_noexcept
		{
			clear();
			append_convert(string, length);
			return *this;
		}

		template<typename OtherCharacterType>
		crstl_constexpr basic_fixed_string& assign_convert(const OtherCharacterType* string) crstl_noexcept
		{
			return assign_convert(string, crstl::string_length(string));
		}

		template<typename OtherCharacterType, int OtherN>
		crstl_constexpr basic_fixed_string& assign_convert(const basic_fixed_string<OtherCharacterType, OtherN>& string) crstl_noexcept
		{
			assign_convert(string.c_str(), string.length());
			return *this;
		}

		// If we append_convert with our own type, just use append, no need for conversion
		crstl_constexpr basic_fixed_string& assign_convert(const_pointer string, size_t length) crstl_noexcept
		{
			assign(string, length);
			return *this;
		}

		crstl_constexpr reference back() crstl_noexcept
		{
			crstl_assert(m_length > 0);
			return m_data[m_length - 1];
		}

		crstl_constexpr const_reference back() const crstl_noexcept
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
			return kCharacterCapacityWithZero * kCharSize;
		}

		crstl_constexpr const_iterator cbegin() const crstl_noexcept { return &m_data[0]; }
		crstl_constexpr const_iterator cend() const crstl_noexcept { return &m_data[0] + m_length; }

		crstl_constexpr void clear() crstl_noexcept
		{
			m_data[0] = '\0';
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

		//---------
		// comparei
		//---------

		crstl_constexpr int comparei(const T* string) const crstl_noexcept
		{
			return crstl::string_comparei(m_data, m_length, string, string_length(string));
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const T* string) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_comparei(m_data + pos, clamp_length(pos, length), string, string_length(string));
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const T* string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_comparei(m_data + pos, clamp_length(pos, length), string + subpos, crstl::string_clamp_length(string_length(string), subpos, sublen));
		}

		crstl_constexpr int comparei(const basic_fixed_string& string) const crstl_noexcept
		{
			return crstl::string_comparei(m_data, m_length, string.m_data, string.m_length);
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const basic_fixed_string& string) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_comparei(m_data + pos, clamp_length(pos, length), string.m_data, string.m_length);
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const basic_fixed_string& string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return crstl::string_comparei(m_data + pos, clamp_length(pos, length), string.m_data + subpos, string.clamp_length(subpos, sublen));
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
		crstl_constexpr size_t find(value_type c, size_t pos = 0) const crstl_noexcept
		{
			crstl_assert(pos <= m_length);
			const_pointer ptr = (const_pointer)string_find_char(m_data + pos, c, m_length - pos);
			return ptr ? (size_t)(ptr - m_data) : npos;
		}

		// Find a const char* string with an offset and a length
		crstl_constexpr size_t find(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			crstl_assert(pos <= m_length);
			const_pointer found_string = string_find(m_data + pos, m_length - pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - m_data) : npos;
		}

		crstl_constexpr size_t find(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string, pos, string_length(needle_string));
		}

		crstl_constexpr size_t find(const basic_fixed_string& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string.m_data, pos, needle_string.m_length);
		}

		//--------------
		// find_first_of
		//--------------

		crstl_constexpr size_t find_first_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			const_pointer found_string = string_find_of(m_data + pos, m_length - pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - m_data) : npos;
		}

		crstl_constexpr size_t find_first_of(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string, pos, string_length(needle_string));
		}

		crstl_constexpr size_t find_first_of(const basic_fixed_string& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string.c_str(), pos, needle_string.length());
		}

		crstl_constexpr size_t find_first_of(value_type c, size_t pos = 0) const crstl_noexcept
		{
			return find(c, pos);
		}

		//-------------
		// find_last_of
		//-------------

		crstl_constexpr size_t find_last_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			pos = pos < m_length ? pos : m_length;
			const_pointer found_string = string_rfind_of(m_data + pos, pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - m_data) : npos;
		}

		crstl_constexpr size_t find_last_of(const_pointer needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string, pos, string_length(needle_string));
		}

		crstl_constexpr size_t find_last_of(const basic_fixed_string& needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string.c_str(), pos, needle_string.length());
		}

		crstl_constexpr size_t find_last_of(value_type c, size_t pos = npos) const crstl_noexcept
		{
			return rfind(c, pos);
		}

		crstl_constexpr reference front() crstl_noexcept { return m_data[0]; }
		crstl_constexpr const_reference front() const crstl_noexcept { return m_data[0]; }

		// Returns the length of the string, in terms of number of characters
		crstl_constexpr size_t length() const crstl_noexcept { return m_length; }

		crstl_constexpr size_t max_size() const crstl_noexcept { return kCharacterCapacity; }

		crstl_constexpr void pop_back() crstl_noexcept
		{
			crstl_assert(m_length > 0);
			m_data[m_length - 1].~T();
			m_length--;
		}

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
			return replace((size_t)(begin - m_data), (size_t)(end - begin), replace_string, crstl::string_length(replace_string));
		}

		crstl_constexpr basic_fixed_string& replace(const_pointer begin, const_pointer end, const_pointer replace_string, size_t replace_length) crstl_noexcept
		{
			return replace((size_t)(begin - m_data), (size_t)(end - begin), replace_string, replace_length);
		}

		crstl_constexpr basic_fixed_string& replace(const_pointer begin, const_pointer end, const basic_fixed_string& replace_string) crstl_noexcept
		{
			return replace((size_t)(begin - m_data), (size_t)(end - begin), replace_string.m_data, (size_t)replace_string.m_length);
		}

		//-------
		// resize
		//-------

		crstl_constexpr void resize(size_t length)
		{
			crstl_assert(length < NumElements);

			// If length is larger than current length, initialize new characters to 0
			if ((size_t)m_length < length)
			{
				for (size_t i = m_length; i < length; ++i)
				{
					m_data[i] = 0;
				}
			}

			m_data[length] = 0;
			m_length = (uint32_t)length;
		}

		crstl_constexpr void resize(size_t length, T c)
		{
			crstl_assert(length < NumElements);

			// If length is larger than current length, initialize new characters to c
			if ((size_t)m_length < length)
			{
				for (size_t i = m_length; i < length; ++i)
				{
					m_data[i] = c;
				}
			}

			m_data[length] = 0;
			m_length = (uint32_t)length;
		}

		//-----
		// rfind
		//-----

		size_t rfind(value_type c, size_t pos = npos) const crstl_noexcept
		{
			pos = pos < m_length ? pos : m_length;
			const_pointer ptr = (const_pointer)string_rfind_char(m_data + pos, c, pos);
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

		//------------
		// starts_with
		//------------
		
		crstl_constexpr bool starts_with(value_type c) const crstl_noexcept { return find(c, 0) == 0; }

		crstl_constexpr size_t starts_with(const_pointer needle_string, size_t needle_length) const crstl_noexcept
		{
			return find(needle_string, 0, needle_length) == 0;
		}

		crstl_constexpr size_t starts_with(const_pointer needle_string) const crstl_noexcept
		{
			return find(needle_string, 0, string_length(needle_string)) == 0;
		}

		crstl_constexpr size_t starts_with(const basic_fixed_string& needle_string) const crstl_noexcept
		{
			return find(needle_string.m_data, 0, needle_string.m_length) == 0;
		}

		crstl_constexpr basic_fixed_string substr(size_t pos, size_t length = npos) const crstl_noexcept
		{
			return basic_fixed_string(*this, pos, length);
		}

		//----------
		// Operators
		//----------

		crstl_constexpr reference operator [](size_t i)
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr const_reference operator [](size_t i) const crstl_noexcept
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

		crstl_constexpr bool operator == (const_pointer string) const crstl_noexcept
		{
			return compare(string) == 0;
		}

		crstl_constexpr bool operator != (const_pointer string) const crstl_noexcept
		{
			return compare(string) != 0;
		}

		crstl_constexpr bool operator == (const basic_fixed_string& string) const crstl_noexcept
		{
			return compare(string) == 0;
		}

		crstl_constexpr bool operator != (const basic_fixed_string& string) const crstl_noexcept
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
				crstl_assert(m_length + replace_difference < kCharacterCapacityWithZero);
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
	template<typename T, int NumElements, int OtherNumElements>
	basic_fixed_string<T, (NumElements > OtherNumElements ? NumElements : OtherNumElements)> operator + (const basic_fixed_string<T, NumElements>& string1, const basic_fixed_string<T, OtherNumElements>& string2)
	{
		return basic_fixed_string<T, (NumElements > OtherNumElements ? NumElements : OtherNumElements)>(string1, string2);
	}

	typedef basic_fixed_string<char, 8> fixed_string8;
	typedef basic_fixed_string<char, 16> fixed_string16;
	typedef basic_fixed_string<char, 32> fixed_string32;
	typedef basic_fixed_string<char, 64> fixed_string64;
	typedef basic_fixed_string<char, 128> fixed_string128;
	typedef basic_fixed_string<char, 256> fixed_string256;
	typedef basic_fixed_string<char, 512> fixed_string512;

	typedef basic_fixed_string<wchar_t, 8> fixed_wstring8;
	typedef basic_fixed_string<wchar_t, 16> fixed_wstring16;
	typedef basic_fixed_string<wchar_t, 32> fixed_wstring32;
	typedef basic_fixed_string<wchar_t, 64> fixed_wstring64;
	typedef basic_fixed_string<wchar_t, 128> fixed_wstring128;
	typedef basic_fixed_string<wchar_t, 256> fixed_wstring256;
	typedef basic_fixed_string<wchar_t, 512> fixed_wstring512;
}