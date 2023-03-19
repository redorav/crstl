#pragma once

#include "config.h"

#include "crstldef.h"

#include "type_utils.h"

// unique_ptr
//
// Simple replacement for std::unique_ptr, functionally the same
// but with a very small compile time footprint
// The move-only semantics remain the same
//
// - unique_ptr doesn't allocate memory, the pointer is supplied externally
//   but it does deallocate memory, so care has to be take to allocate and 
//   deallocate in a consistent manner

crstl_module_export namespace crstl
{
	template<typename UniquePtr, typename T>
	class unique_ptr_base
	{
	public:

		typedef T element_type;
		typedef T* pointer;

		crstl_constexpr unique_ptr_base() crstl_noexcept : m_ptr(nullptr) {}

		crstl_constexpr explicit unique_ptr_base(T* ptr) crstl_noexcept : m_ptr(ptr) {}

		crstl_constexpr unique_ptr_base(crstl::nullptr_t) crstl_noexcept : m_ptr(nullptr) {}

		crstl_constexpr unique_ptr_base(unique_ptr_base&& uptr) crstl_noexcept
		{
			m_ptr = uptr.m_ptr;
			uptr.m_ptr = nullptr;
		}

		crstl_constexpr unique_ptr_base& operator = (crstl::nullptr_t) crstl_noexcept
		{
			static_cast<UniquePtr&>(*this).reset(nullptr);
			return *this;
		}

		crstl_constexpr unique_ptr_base& operator = (unique_ptr_base&& uptr) crstl_noexcept
		{
			m_ptr = uptr.m_ptr;
			uptr.m_ptr = nullptr;
			return *this;
		}

		crstl_constexpr T* operator ->() const crstl_noexcept
		{
			return m_ptr;
		}

		bool operator!() const crstl_noexcept
		{
			return (m_ptr == nullptr);
		}

		~unique_ptr_base() crstl_noexcept
		{
			static_cast<UniquePtr&>(*this).reset(nullptr);
		}

		typedef T* (unique_ptr_base<UniquePtr, T>::* boolean)() const;

		operator boolean() const crstl_noexcept
		{
			// Return anything that isn't easily castable but is guaranteed to be non-null, such as the get function pointer
			return m_ptr ? &unique_ptr_base<UniquePtr, T>::get : nullptr;
		}

		T* get() const
		{
			return m_ptr;
		}

		bool operator == (const unique_ptr_base& other) { return m_ptr == other.m_ptr; }
		bool operator != (const unique_ptr_base& other) { return m_ptr != other.m_ptr; }
		bool operator <  (const unique_ptr_base& other) { return m_ptr <  other.m_ptr; }
		bool operator <= (const unique_ptr_base& other) { return m_ptr <= other.m_ptr; }
		bool operator >  (const unique_ptr_base& other) { return m_ptr >  other.m_ptr; }
		bool operator >= (const unique_ptr_base& other) { return m_ptr >= other.m_ptr; }

		bool operator == (pointer ptr) { return m_ptr == ptr; }
		bool operator != (pointer ptr) { return m_ptr != ptr; }
		bool operator <  (pointer ptr) { return m_ptr <  ptr; }
		bool operator <= (pointer ptr) { return m_ptr <= ptr; }
		bool operator >  (pointer ptr) { return m_ptr >  ptr; }
		bool operator >= (pointer ptr) { return m_ptr >= ptr; }

		bool operator == (crstl::nullptr_t) { return m_ptr == nullptr; }
		bool operator != (crstl::nullptr_t) { return m_ptr != nullptr; }
		bool operator <  (crstl::nullptr_t) { return m_ptr <  nullptr; }
		bool operator <= (crstl::nullptr_t) { return m_ptr <= nullptr; }
		bool operator >  (crstl::nullptr_t) { return m_ptr >  nullptr; }
		bool operator >= (crstl::nullptr_t) { return m_ptr >= nullptr; }

	protected:

		pointer m_ptr;

	private:

		crstl_constexpr unique_ptr_base(const unique_ptr_base& uptr) crstl_noexcept;
		crstl_constexpr unique_ptr_base& operator = (const unique_ptr_base& uptr) crstl_noexcept;
		crstl_constexpr unique_ptr_base& operator = (T* ptr) crstl_noexcept;
	};

	// unique_ptr for scalar types

	template<typename T>
	class unique_ptr : public unique_ptr_base<unique_ptr<T>, T>
	{
	public:

		typedef unique_ptr_base<unique_ptr<T>, T> base;

		crstl_constexpr unique_ptr() crstl_noexcept : base() {}

		crstl_constexpr explicit unique_ptr(T* ptr) crstl_noexcept : base(ptr) {}

		crstl_constexpr unique_ptr(crstl::nullptr_t) crstl_noexcept : base(nullptr) {}

		crstl_constexpr unique_ptr(unique_ptr&& uptr) crstl_noexcept : base(crstl::move(uptr)) {}

		crstl_constexpr unique_ptr& operator = (crstl::nullptr_t) crstl_noexcept { base::operator = (nullptr); return *this; }

		crstl_constexpr unique_ptr& operator = (unique_ptr&& uptr) crstl_noexcept { base::operator = (crstl::move(uptr)); return *this; }

		crstl_constexpr void reset(crstl::nullptr_t) crstl_noexcept
		{
			delete base::m_ptr;
			base::m_ptr = nullptr;
		}

		crstl_constexpr void reset(typename base::pointer ptr = base::pointer()) crstl_noexcept
		{
			delete base::m_ptr;
			base::m_ptr = ptr;
		}
	};

	// unique_ptr for arrays

	template<typename T>
	class unique_ptr<T[]> : public unique_ptr_base<unique_ptr<T[]>, T>
	{
	public:

		typedef unique_ptr_base<unique_ptr<T[]>, T> base;

		crstl_constexpr unique_ptr() crstl_noexcept : base() {}

		crstl_constexpr explicit unique_ptr(T* ptr) crstl_noexcept : base(ptr) {}

		crstl_constexpr unique_ptr(crstl::nullptr_t) crstl_noexcept : base(nullptr) {}

		crstl_constexpr unique_ptr(unique_ptr&& uptr) crstl_noexcept : base(crstl::move(uptr)) {}

		crstl_constexpr unique_ptr& operator = (crstl::nullptr_t) crstl_noexcept { base::operator = (nullptr); return *this; }

		crstl_constexpr unique_ptr& operator = (unique_ptr&& uptr) crstl_noexcept { base::operator = (crstl::move(uptr)); return *this; }

		crstl_constexpr void reset(crstl::nullptr_t) crstl_noexcept
		{
			delete[] base::m_ptr;
			base::m_ptr = nullptr;
		}

		crstl_constexpr void reset(typename base::pointer ptr = base::pointer()) crstl_noexcept
		{
			delete[] base::m_ptr;
			base::m_ptr = ptr;
		}
	};
};