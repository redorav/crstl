#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/utility/string_common.h"

// crstl::string_view
//
// Replacement for std::string_view
//

crstl_module_export namespace crstl
{
	template<typename CharT>
	class basic_string_view
	{
	public:

		typedef CharT        value_type;
		typedef CharT&       reference;
		typedef const CharT& const_reference;
		typedef CharT*       pointer;
		typedef const CharT* const_pointer;
		typedef const CharT* iterator;
		typedef const CharT* const_iterator;
		typedef size_t       size_type;

		static const crstl_constexpr size_type npos = (size_type)-1;

		basic_string_view() crstl_noexcept : m_data(nullptr), m_length(0) {}

		basic_string_view(const_pointer ptr) crstl_noexcept : m_data(ptr), m_length(string_length(ptr)) {}

		basic_string_view(const_pointer ptr, size_type size) : m_data(ptr), m_length(size) {}

		basic_string_view(const_pointer begin, const_pointer end) : m_data(begin), m_length(end - begin) {}

		crstl_constexpr14 CharT& at(size_type i) crstl_noexcept
		{
			return crstl_assert(i < m_length), m_data[i];
		}

		crstl_constexpr const CharT& at(size_type i) const crstl_noexcept
		{
			return crstl_assert(i < m_length), m_data[i];
		}

		crstl_constexpr14 CharT& back() crstl_noexcept
		{
			return crstl_assert(m_length > 0), m_data[m_length - 1];
		}

		crstl_constexpr const CharT& back() const crstl_noexcept
		{
			return crstl_assert(m_length > 0), m_data[m_length - 1];
		}

		crstl_constexpr14 iterator begin() crstl_noexcept { return m_data; }
		crstl_constexpr const_iterator begin() const crstl_noexcept { return m_data; }

		crstl_constexpr const_iterator cbegin() const crstl_noexcept { return m_data; }
		crstl_constexpr const_iterator cend() const crstl_noexcept { return m_data + m_length; }

		//--------
		// compare
		//--------

		crstl_constexpr int compare(const CharT* string) const crstl_noexcept
		{
			return crstl::string_compare(m_data, m_length, string, string_length(string));
		}

		crstl_constexpr int compare(const CharT* string, size_t string_length) const crstl_noexcept
		{
			return crstl::string_compare(m_data, m_length, string, string_length);
		}

		crstl_constexpr int compare(size_t pos, size_t length, const CharT* string) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_compare(m_data + pos, clamp_length(pos, length), string, string_length(string));
		}

		crstl_constexpr int compare(size_t pos, size_t length, const CharT* string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_compare(m_data + pos, clamp_length(pos, length), string + subpos, crstl::string_clamp_length(string_length(string), subpos, sublen));
		}

		crstl_constexpr int compare(const basic_string_view& sv) const crstl_noexcept
		{
			return crstl::string_compare(m_data, m_length, sv.m_data, sv.m_length);
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_string_view& sv) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_compare(m_data + pos, clamp_length(pos, length), sv.m_data, sv.m_length);
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_string_view& sv, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_compare(m_data + pos, clamp_length(pos, length), sv.m_data + subpos, sv.clamp_length(subpos, sublen));
		}

		//---------
		// comparei
		//---------

		crstl_constexpr int comparei(const CharT* string) const crstl_noexcept
		{
			return crstl::string_comparei(m_data, m_length, string, string_length(string));
		}

		crstl_constexpr int comparei(const CharT* string, size_t string_length) const crstl_noexcept
		{
			return crstl::string_comparei(m_data, m_length, string, string_length);
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const CharT* string) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_comparei(m_data + pos, clamp_length(pos, length), string, string_length(string));
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const CharT* string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_comparei(m_data + pos, clamp_length(pos, length), string + subpos, crstl::string_clamp_length(string_length(string), subpos, sublen));
		}

		crstl_constexpr int comparei(const basic_string_view& sv) const crstl_noexcept
		{
			return crstl::string_comparei(m_data, m_length, sv.m_data, sv.m_length);
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const basic_string_view& sv) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_comparei(m_data + pos, clamp_length(pos, length), sv.m_data, sv.m_length);
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const basic_string_view& sv, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), crstl::string_comparei(m_data + pos, clamp_length(pos, length), sv.m_data + subpos, sv.clamp_length(subpos, sublen));
		}

		crstl_constexpr14 pointer data() crstl_noexcept { return m_data; }
		crstl_constexpr const_pointer data() const crstl_noexcept { return m_data; }

		crstl_nodiscard
		crstl_constexpr bool empty() const crstl_noexcept { return m_length == 0; }

		crstl_constexpr14 iterator end() crstl_noexcept { return m_data + m_length; }
		crstl_constexpr const_iterator end() const crstl_noexcept { return m_data + m_length; }

		crstl_constexpr bool ends_with(value_type c) const { return find(c, 0) == (m_length - 1); }

		crstl_constexpr bool ends_with(const basic_string_view& sv) const { return find(sv.m_data, m_length - sv.m_length) == (m_length - sv.m_length); }

		//-----
		// find
		//-----

		size_t find(value_type c, size_t pos = 0) const crstl_noexcept
		{
			if (pos < m_length)
			{
				const_pointer found_char = (const_pointer)string_find_char(m_data + pos, c, m_length - pos);
				return found_char ? (size_t)(found_char - m_data) : npos;
			}
			else
			{
				return npos;
			}
		}

		size_t find(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			if (pos < m_length)
			{
				const_pointer found_string = string_find(m_data + pos, m_length - pos, needle_string, needle_length);
				return found_string ? (size_t)(found_string - m_data) : npos;
			}
			else
			{
				return npos;
			}
		}

		size_t find(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string, pos, string_length(needle_string));
		}

		size_t find(const basic_string_view& needle_sv, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_sv.m_data, pos, needle_sv.m_length);
		}

		//--------------
		// find_first_of
		//--------------

		crstl_constexpr14 size_t find_first_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			size_t current_length = length();
			const_pointer current_data = data();

			if (pos < current_length)
			{
				const_pointer found_string = crstl::string_find_of(current_data + pos, current_length - pos, needle_string, needle_length);
				return found_string ? (size_t)(found_string - current_data) : npos;
			}
			else
			{
				return npos;
			}
		}

		crstl_constexpr14 size_t find_first_of(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr14 size_t find_first_of(const basic_string_view& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string.data(), pos, needle_string.length());
		}

		crstl_constexpr14 size_t find_first_of(CharT c, size_t pos = 0) const crstl_noexcept
		{
			return find(c, pos);
		}

		//-------------
		// find_last_of
		//-------------

		crstl_constexpr14 size_t find_last_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			pos = pos < length() ? pos : length();
			const_pointer found_string = crstl::string_rfind_of(data() + pos, pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - data()) : npos;
		}

		crstl_constexpr14 size_t find_last_of(const_pointer needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr14 size_t find_last_of(const basic_string_view& needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string.data(), pos, needle_string.length());
		}

		crstl_constexpr14 size_t find_last_of(CharT c, size_t pos = npos) const crstl_noexcept
		{
			return rfind(c, pos);
		}

		crstl_constexpr14 CharT& front() crstl_noexcept { return m_data; }
		crstl_constexpr const CharT& front() const crstl_noexcept { return m_data; }

		// Returns the length of the string, in terms of number of characters
		crstl_constexpr size_type length() const crstl_noexcept { return m_length; }

		crstl_constexpr size_type max_size() const crstl_noexcept { return (size_type)-1; }

		crstl_constexpr size_type size() const crstl_noexcept { return length(); }

		crstl_constexpr bool starts_with(value_type c) const { return find(c, 0) == 0; }

		crstl_constexpr bool starts_with(const basic_string_view& sv) const { return find(sv.m_data, 0) == 0; }

		crstl_constexpr basic_string_view substr(size_t pos, size_t length = npos) const crstl_noexcept
		{
			return crstl_assert(pos < m_length), basic_string_view(m_data + pos, clamp_length(pos, length));
		}

		//----------
		// Operators
		//----------

		crstl_constexpr14 CharT& operator [](size_type i)
		{
			return crstl_assert(i < m_length), m_data[i];
		}

		crstl_constexpr const CharT& operator [](size_type i) const crstl_noexcept
		{
			return crstl_assert(i < m_length), m_data[i];
		}

		crstl_constexpr bool operator == (const basic_string_view& sv) const crstl_noexcept
		{
			return compare(sv) == 0;
		}

		crstl_constexpr bool operator != (const basic_string_view& sv) const crstl_noexcept
		{
			return compare(sv) != 0;
		}

	private:

		// Given a position and a length, return the length that fits the string
		crstl_constexpr size_type clamp_length(size_type pos, size_type length) const crstl_noexcept
		{
			return crstl::string_clamp_length(m_length, pos, length);
		}

		const_pointer m_data;

		size_type m_length;
	};

	typedef basic_string_view<char> string_view;

	typedef basic_string_view<wchar_t> wstring_view;
};