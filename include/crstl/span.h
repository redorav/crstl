#pragma once

#include "crstl/config.h"

#include "crstl/type_utils.h"

// crstl::span
//
// Replacement for std::span
//

crstl_module_export namespace crstl
{
	template<typename T>
	class span
	{
	public:

		typedef T                            element_type;
		typedef typename remove_cv<T>:: type value_type;
		typedef size_t                       size_type;
		typedef T&                           reference;
		typedef const T&                     const_reference;
		typedef T*                           pointer;
		typedef const T*                     const_pointer;
		typedef T*                           iterator;
		typedef const T*                     const_iterator;

		span() : m_data(nullptr), m_length(0) {}

		span(T* data, size_t length) : m_data(data), m_length(length) {}

		T& at(size_t i) { crstl_assert(i < m_length); return m_data[i]; }
		const T& at(size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		T& back() { crstl_assert(m_length > 0); return m_data[m_length - 1]; }
		const T& back() const { crstl_assert(m_length > 0); return m_data[m_length - 1]; }

		iterator begin() { return &m_data[0]; }
		const_iterator begin() const { return &m_data[0]; }
		const_iterator cbegin() const { return &m_data[0]; }

		pointer data() { return &m_data[0]; }
		const_pointer data() const { return &m_data[0]; }

		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		iterator end() { return &m_data[0] + m_length; }
		const_iterator end() const { return &m_data[0] + m_length; }
		const_iterator cend() const { return &m_data[0] + m_length; }

		span first(size_t length) const { crstl_assert(length < m_length); return subspan(m_data + length, m_length - length); }

		T& front() { return m_data[0]; }
		const T& front() const { return m_data[0]; }

		span last(size_t length) const { crstl_assert(length < m_length); return subspan(m_data + (m_length - length), length); }

		size_t size() const { return m_length; }

		size_t size_bytes() const { return m_length * sizeof(T); }

		span subspan(size_t offset, size_t length) const
		{
			crstl_assert(offset < m_length);
			return span(m_data + offset, length);
		}

		T& operator [] (size_t i) { crstl_assert(i < m_length); return m_data[i]; }

		const T& operator [] (size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

	private:

		T* m_data;

		size_t m_length;
	};
};