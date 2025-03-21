#pragma once

#include "crstl/config.h"
#include "crstl/type_utils.h"
#include "crstl/forward_declarations.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

// crstl::span
//
// Replacement for std::span
//

crstl_module_export namespace crstl
{
	template<typename T, size_t Size>
	class span_base
	{
	protected:

		// Use this to allow child class to use m_length as though it was the member variable
		enum t : size_t
		{
			m_length = Size
		};

		span_base(T* data)
			: m_data(data)
		{}

		T* m_data;
	};

	template<typename T>
	class span_base <T, dynamic_extent>
	{
	protected:

		span_base(T* data, size_t length)
			: m_data(data)
			, m_length(length)
		{}

		T* m_data;
		size_t m_length;
	};

	template<typename T, size_t Size>
	class span : public span_base<T, Size>
	{
	public:

		typedef span_base<T, Size>           base_type;
		typedef T                            element_type;
		typedef typename remove_cv<T>:: type value_type;
		typedef size_t                       size_type;
		typedef T&                           reference;
		typedef const T&                     const_reference;
		typedef T*                           pointer;
		typedef const T*                     const_pointer;
		typedef T*                           iterator;
		typedef const T*                     const_iterator;

		using base_type::m_data;
		using base_type::m_length;

		template <size_t N = Size, typename = typename crstl::enable_if<(N == crstl::dynamic_extent)>::type>
		span(T* data, size_t length) crstl_noexcept : base_type(data, length) {}
		
		template <size_t N = Size, typename = typename crstl::enable_if<(N != crstl::dynamic_extent)>::type>
		span(T* data) crstl_noexcept : base_type(data) {}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		template <size_t N = Size, typename = typename crstl::enable_if<(N == crstl::dynamic_extent)>::type>
		crstl_constexpr14 span(std::initializer_list<T> ilist) crstl_noexcept : base_type(ilist.begin(), ilist.size()) {}

#endif

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
	};
};