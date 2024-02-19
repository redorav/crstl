#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

// crstl::array
//
// Replacement for std::array
//

crstl_module_export namespace crstl
{
	template<typename T, size_t N = 1>
	class array
	{
	public:

		typedef array<T, N>   this_type;
		typedef T&            reference;
		typedef const T&      const_reference;
		typedef T*            pointer;
		typedef const T*      const_pointer;
		typedef T*            iterator;
		typedef const T*      const_iterator;
		typedef uint32_t      length_type;

		crstl_constexpr14 iterator begin() { return m_data; }
		crstl_constexpr const_iterator begin() const { return m_data; }
		crstl_constexpr const_iterator cbegin() const { return m_data; }

		crstl_constexpr14 iterator end() { return m_data + N; }
		crstl_constexpr const_iterator end() const { return m_data + N; }
		crstl_constexpr const_iterator cend() const { return m_data + N; }

		crstl_nodiscard
		crstl_constexpr bool empty() const { return N == 0; }

		crstl_constexpr size_t size() const { return N; }
		crstl_constexpr size_t max_size() const { return N; }

		crstl_constexpr14 T* data() { return m_data; }
		crstl_constexpr const T* data() const { return m_data; }

		crstl_constexpr14 T& operator[](size_t i) { return crstl_assert(i < N), m_data[i]; }
		crstl_constexpr const T& operator[](size_t i) const { return crstl_assert(i < N), m_data[i]; }

		crstl_constexpr14 T& at(size_t i) { return crstl_assert(i < N), m_data[i]; }
		crstl_constexpr const T& at(size_t i) const { return crstl_assert(i < N), m_data[i]; }

		// Public to allow braced initialization
		T m_data[N ? N : 1];
	};
}