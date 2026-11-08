#pragma once

#include "crstl/config.h"

// crstl::range_ptr
//
// Use this as you would a pointer, but with bound checks. Allows pointer arithmetic, dereferencing, taking the address of it,
// comparison, implicit casting to a pointer, etc

crstl_module_export namespace crstl
{
	template<typename T>
	class range_ptr
	{
	public:

		range_ptr(T* data, size_t length) crstl_noexcept
			: m_current(data)
			, m_begin(data)
			, m_length(length)
		{
			crstl_assert(uintptr_t(m_begin) < uintptr_t(size_t(-1)) - length);
		}

		// Address of

		crstl_constexpr14 T** operator & ()
		{
			return &m_current;
		}

		crstl_constexpr const T** operator & () const
		{
			return &m_current;
		}

		// Dereference

		crstl_constexpr14 T& operator * ()
		{
			return *m_current;
		}

		crstl_constexpr const T& operator * () const
		{
			return *m_current;
		}

		// Preincrement

		crstl_constexpr14 range_ptr& operator ++ ()
		{
			crstl_assert(m_current < m_begin + m_length);
			m_current++;
			return *this;
		}

		// Postincrement

		crstl_constexpr14 range_ptr operator ++ (int)
		{
			crstl_assert(m_current < m_begin + m_length);
			range_ptr temp(*this);
			m_current++;
			return temp;
		}

		// Predecrement

		crstl_constexpr14 range_ptr& operator -- ()
		{
			crstl_assert(m_current > m_begin);
			m_current--;
			return *this;
		}

		// Postdecrement

		crstl_constexpr14 range_ptr operator -- (int)
		{
			crstl_assert(m_current > m_begin);
			range_ptr temp(*this);
			m_current--;
			return temp;
		}

		// General arithmetic

		crstl_constexpr14 range_ptr& operator += (size_t increment)
		{
			crstl_assert(m_current < m_begin + increment + m_length);
			m_current += increment;
			return *this;
		}

		crstl_constexpr14 range_ptr& operator -= (size_t increment)
		{
			crstl_assert(m_current >= m_begin + increment);
			m_current -= increment;
			return *this;
		}

		// Implicit cast to pointer

		crstl_constexpr14 operator T* ()
		{
			return m_current;
		}

		crstl_constexpr operator T* () const
		{
			return m_current;
		}

		// Comparison operators

		crstl_constexpr bool operator == (T* ptr) const
		{
			return m_current == ptr;
		}

		crstl_constexpr bool operator != (T* ptr) const
		{
			return m_current != ptr;
		}

		crstl_constexpr bool operator > (T* ptr) const
		{
			return m_current > ptr;
		}

		crstl_constexpr bool operator >= (T* ptr) const
		{
			return m_current >= ptr;
		}

		crstl_constexpr bool operator < (T* ptr) const
		{
			return m_current < ptr;
		}

		crstl_constexpr bool operator <= (T* ptr) const
		{
			return m_current <= ptr;
		}

		// Array

		crstl_constexpr14 T& operator[] (size_t i)
		{
			return crstl_assert(m_current + i < m_begin + m_length), m_current[i];
		}

		crstl_constexpr const T& operator[](size_t i) const
		{
			return crstl_assert(m_current + i < m_begin + m_length), m_current[i];
		}

	private:

		T* m_current;

		T* m_begin;

		size_t m_length;
	};
}