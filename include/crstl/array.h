#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

crstl_module_export namespace crstl
{
	template<typename T, size_t N = 1>
	class array
	{
	public:

		crstl_constexpr14 T* begin() { return m_data; }
		crstl_constexpr const T* begin() const { return m_data; }
		crstl_constexpr const T* cbegin() const { return m_data; }

		crstl_constexpr14 T* end() { return m_data + N; }
		crstl_constexpr const T* end() const { return m_data + N; }
		crstl_constexpr const T* cend() const { return m_data + N; }

		crstl_nodiscard
		crstl_constexpr bool empty() const { return N == 0; }

		crstl_constexpr size_t size() const { return N; }
		crstl_constexpr size_t max_size() const { return N; }

		crstl_constexpr14 T* data() { return m_data; }
		crstl_constexpr const T* data() const { return m_data; }

		crstl_constexpr14 T& operator[](size_t i) { return m_data[i]; }
		crstl_constexpr const T& operator[](size_t i) const { return m_data[i]; }

		crstl_constexpr14 T& at(size_t i) { return m_data[i]; }
		crstl_constexpr const T& at(size_t i) const { return m_data[i]; }

		// Public to allow braced initialization
		T m_data[N ? N : 1];
	};
}