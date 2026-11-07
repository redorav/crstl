#pragma once

#include "crstl/config.h"

// crstl::range_ptr
//
// Use this as you would a pointer, but with bound checks. Allows pointer arithmetic, dereferencing, taking the address of it,
// comparison, implicit casting to a pointer, etc

template<typename T>
class range_ptr
{
public:

	range_ptr(T* data, size_t length)
		: m_current(data)
		, m_begin(data)
		, m_length(length)
	{
		crstl_assert(uintptr_t(m_begin) < uintptr_t(size_t(-1)) - length);
	}

	// Address operator
	T** operator & ()
	{
		return &m_current;
	}

	const T* operator&() const
	{
		return m_current;
	}

	// Dereference operator
	T& operator*() const
	{
		return *m_current;
	}

	// Preincrement
	range_ptr& operator ++ ()
	{
		crstl_assert(m_current < m_begin + m_length);
		m_current++;
		return *this;
	}

	// Postincrement
	range_ptr operator ++ (int)
	{
		crstl_assert(m_current < m_begin + m_length);
		range_ptr temp(*this);
		m_current++;
		return temp;
	}

	// Predecrement
	range_ptr& operator -- ()
	{
		crstl_assert(m_current > m_begin);
		m_current--;
		return *this;
	}

	// Postdecrement
	range_ptr operator -- (int)
	{
		crstl_assert(m_current > m_begin);
		range_ptr temp(*this);
		m_current--;
		return temp;
	}

	// General arithmetic
	range_ptr& operator += (size_t increment)
	{
		crstl_assert(m_current < m_begin + increment + m_length);
		m_current += increment;
		return *this;
	}

	range_ptr& operator -= (size_t increment)
	{
		crstl_assert(m_current > m_begin + increment);
		m_current -= increment;
		return *this;
	}

	// Implicit cast to pointer
	operator T* ()
	{
		return m_current;
	}

	// Comparison operators
	bool operator == (T* ptr)
	{
		return m_current == ptr;
	}

	bool operator != (T* ptr)
	{
		return m_current != ptr;
	}

	bool operator > (T * ptr)
	{
		return m_current > ptr;
	}

	bool operator >= (T* ptr)
	{
		return m_current >= ptr;
	}

	bool operator < (T* ptr)
	{
		return m_current < ptr;
	}

	bool operator <= (T* ptr)
	{
		return m_current <= ptr;
	}

private:

	T* m_current;

	T* m_begin;

	size_t m_length;
};