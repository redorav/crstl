#pragma once

#include "config.h"

namespace crstl
{
	template<typename T, size_t N = 1>
	class array
	{
	public:

		crstl_constexpr T* begin() { return m_contents; }
		crstl_constexpr const T* begin() const { return m_contents; }
		crstl_constexpr const T* cbegin() const { return m_contents; }

		crstl_constexpr T* end() { return m_contents + N; }
		crstl_constexpr const T* end() const { return m_contents + N; }
		crstl_constexpr const T* cend() const { return m_contents + N; }

		crstl_constexpr bool empty() const { return N == 0; }
		crstl_constexpr size_t size() const { return N; }
		crstl_constexpr size_t max_size() const { return N; }

		crstl_constexpr T* data() { return m_contents; }
		crstl_constexpr T* const data() const { return m_contents; }

		crstl_constexpr T& operator[](size_t i) { return m_contents[i]; }
		crstl_constexpr const T& operator[](size_t i) const { return m_contents[i]; }

		crstl_constexpr T& at(size_t i) { return m_contents[i]; }
		crstl_constexpr const T& at(size_t i) const { return m_contents[i]; }

		// Public to allow braced initialization
		T m_contents[N ? N : 1];
	};
}