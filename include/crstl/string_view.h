#pragma once

#include "config.h"

#include <stddef.h>

namespace crstl
{
	template<typename T>
	class basic_string_view
	{
	public:

		typedef T        value_type;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef const T* iterator;
		typedef const T* const_iterator;
		typedef size_t   size_type;

		static const crstl_constexpr size_type npos = (size_type)-1;

		basic_string_view() crstl_noexcept : m_data(nullptr), m_length(0) {}

		basic_string_view(const_pointer ptr) crstl_noexcept : m_data(ptr), m_length(string_length(ptr)) {}

		basic_string_view(const_pointer ptr, size_type size) : m_data(ptr), m_length(size) {}

		basic_string_view(const_pointer begin, const_pointer end) : m_data(begin), m_length(end - begin) {}

		crstl_constexpr reference at(size_type i) crstl_noexcept
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr const_reference at(size_type i) const crstl_noexcept
		{
			crstl_assert(i < m_length);
			return m_data[i];
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

		crstl_constexpr iterator begin() crstl_noexcept { return m_data; }
		crstl_constexpr const_iterator begin() const crstl_noexcept { return m_data; }

		crstl_constexpr const_iterator cbegin() const crstl_noexcept { return m_data; }
		crstl_constexpr const_iterator cend() const crstl_noexcept { return m_data + m_length; }

		crstl_constexpr int compare(const basic_string_view& sv) const crstl_noexcept
		{
			return crstl::string_compare(m_data, m_length, sv.m_data, sv.m_length);
		}

		crstl_constexpr pointer data() crstl_noexcept { return m_data; }
		crstl_constexpr const_pointer data() const crstl_noexcept { return m_data; }

		crstl_constexpr bool empty() const crstl_noexcept { return m_length == 0; }

		crstl_constexpr iterator end() crstl_noexcept { return m_data + m_length; }
		crstl_constexpr const_iterator end() const crstl_noexcept { return m_data + m_length; }

		crstl_constexpr bool ends_with(value_type c) { return find(c, 0) == (m_length - 1); }

		crstl_constexpr bool ends_with(const basic_string_view& sv) { return find(sv.m_data, m_length - sv.m_length) == (m_length - sv.m_length); }

		//-----
		// find
		//-----

		size_t find(value_type c, size_t pos = 0) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			const_pointer ptr = (const_pointer)string_find_char(m_data + pos, c, m_length - pos);
			return ptr ? (size_t)(ptr - m_data) : npos;
		}

		size_t find(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			crstl_assert(pos < m_length);

			const_pointer found_string = string_find(m_data + pos, m_length - pos, needle_string, needle_length);

			if (found_string)
			{
				return (size_t)(found_string - m_data);
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

		crstl_constexpr reference front() crstl_noexcept { m_data; }
		crstl_constexpr const_reference front() const crstl_noexcept { m_data; }

		// Returns the length of the string, in terms of number of characters
		crstl_constexpr size_type length() const crstl_noexcept { return m_length; }

		crstl_constexpr size_type max_size() const crstl_noexcept { return (size_type)-1; }

		crstl_constexpr size_type size() const crstl_noexcept { return length(); }

		crstl_constexpr bool starts_with(value_type c) { return find(c, 0) == 0; }

		crstl_constexpr bool starts_with(const basic_string_view& sv) { return find(sv.m_data, 0) == 0; }

		crstl_constexpr basic_string_view substr(size_t pos, size_t length = npos) const crstl_noexcept
		{
			crstl_assert(pos < m_length);
			return basic_string_view(m_data + pos, clamp_length(pos, length));
		}

		//----------
		// Operators
		//----------

		crstl_constexpr reference operator [](size_type i)
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr const_reference operator [](size_type i) const crstl_noexcept
		{
			crstl_assert(i < m_length);
			return m_data[i];
		}

		crstl_constexpr bool operator == (const basic_string_view& sv) crstl_noexcept
		{
			return compare(sv) == 0;
		}

		crstl_constexpr bool operator != (const basic_string_view& sv) crstl_noexcept
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