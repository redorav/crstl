#pragma once

#include "config.h"

#include "crstldef.h"

// unique_ptr
//
// This is a simple replacement for std::unique_ptr, functionally the same
// but with a very small compile time footprint
// The move-only semantics remain the same
//
// unique_ptr doesn't allocate memory, the pointer is supplied externally
// but it does deallocate memory, so care has to be take to allocate and 
// deallocate in a consistent manner

namespace crstl
{
	template<typename T>
	class unique_ptr
	{
	public:

		typedef T element_type;

		crstl_constexpr unique_ptr() crstl_noexcept : m_ptr(nullptr) {}

		crstl_constexpr explicit unique_ptr(T* ptr) crstl_noexcept : m_ptr(ptr) {}

		crstl_constexpr unique_ptr(crstl::nullptr_t) crstl_noexcept : m_ptr(nullptr) {}

		crstl_constexpr unique_ptr(unique_ptr&& uptr) crstl_noexcept
		{
			m_ptr = uptr.m_ptr;
			uptr.m_ptr = nullptr;
		}

		crstl_constexpr unique_ptr& operator = (crstl::nullptr_t) crstl_noexcept
		{
			destroy();
			return *this;
		}

		crstl_constexpr unique_ptr& operator = (unique_ptr&& uptr) crstl_noexcept
		{
			m_ptr = uptr.m_ptr;
			uptr.m_ptr = nullptr;
			return *this;
		}

		crstl_constexpr T* operator ->() const crstl_noexcept
		{
			return m_ptr;
		}

		~unique_ptr() crstl_noexcept
		{
			destroy();
		}

	private:

		crstl_constexpr unique_ptr(const unique_ptr& uptr) crstl_noexcept;
		crstl_constexpr unique_ptr& operator = (const unique_ptr& uptr) crstl_noexcept;
		crstl_constexpr unique_ptr& operator = (T* ptr) crstl_noexcept;

		void destroy() crstl_noexcept
		{
			if (m_ptr)
			{
				delete m_ptr;
				m_ptr = nullptr;
			}
		}

		T* m_ptr;
	};
};