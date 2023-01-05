#pragma once

#include "config.h"

#include "atomic.h"

namespace crstl
{
	template<typename T>
	void intrusive_ptr_add_ref(T* ptr)
	{
		ptr->add_ref();
	}

	template<typename T>
	void intrusive_ptr_release(T* ptr)
	{
		int32_t currentRef = ptr->release_ref();

		if (currentRef == 0)
		{
			ptr->template delete_callback<T>();
		}
	}

	template<typename T>
	class intrusive_ptr
	{
	public:

		intrusive_ptr() = default;

		intrusive_ptr(T* ptr) crstl_noexcept : m_ptr(ptr)
		{
			if (ptr)
			{
				intrusive_ptr_add_ref(ptr);
			}
		}

		intrusive_ptr(const intrusive_ptr<T>& ptr) crstl_noexcept : m_ptr(ptr.m_ptr)
		{
			if (m_ptr != nullptr)
			{
				intrusive_ptr_add_ref(m_ptr);
			}
		}

		intrusive_ptr(intrusive_ptr<T>&& ptr) crstl_noexcept : m_ptr(ptr.m_ptr)
		{
			ptr.m_ptr = nullptr;
		}

		~intrusive_ptr() crstl_noexcept
		{
			if (m_ptr)
			{
				intrusive_ptr_release(m_ptr);
			}
		}

		intrusive_ptr& operator = (T* ptr) crstl_noexcept
		{
			if (m_ptr != ptr)
			{
				T* const ptr_temp = m_ptr;

				// Add a reference to the new pointer
				if (ptr)
				{
					intrusive_ptr_add_ref(ptr);
				}

				// Assign to member pointer
				m_ptr = ptr;

				// Release reference from the old pointer we used to hold on to
				if (ptr_temp)
				{
					intrusive_ptr_release(ptr_temp);
				}
			}

			return *this;
		}

		intrusive_ptr& operator = (const intrusive_ptr<T>& ptr) crstl_noexcept
		{
			if (m_ptr != ptr.m_ptr)
			{
				T* const ptr_temp = m_ptr;

				// Add a reference to the new pointer
				if (ptr.m_ptr)
				{
					intrusive_ptr_add_ref(ptr.m_ptr);
				}

				// Assign to member pointer
				m_ptr = ptr.m_ptr;

				// Release reference from the old pointer we used to hold on to
				if (ptr_temp)
				{
					intrusive_ptr_release(ptr_temp);
				}
			}

			return *this;
		}

		intrusive_ptr& operator = (intrusive_ptr<T>&& ptr) crstl_noexcept
		{
			if (m_ptr != ptr.m_ptr)
			{
				T* const ptr_temp = m_ptr;

				// Assign to member pointer
				m_ptr = ptr.m_ptr;

				// Release reference from the old pointer we used to hold on to
				if (ptr_temp)
				{
					intrusive_ptr_release(ptr_temp);
				}

				ptr.m_ptr = nullptr;
			}

			return *this;
		}

		T* get() const crstl_noexcept { return m_ptr; }

		T* operator ->() const crstl_noexcept
		{
			return m_ptr;
		}

		typedef T* (intrusive_ptr<T>::*boolean)() const;

		operator boolean() const crstl_noexcept
		{
			// Return anything that isn't easily castable but is guaranteed to be non-null, such as the get function pointer
			return m_ptr ? &intrusive_ptr<T>::get : nullptr;
		}

		bool operator!() const crstl_noexcept
		{
			return (m_ptr == nullptr);
		}

	private:

		T* m_ptr = nullptr;
	};

	// Simple interface for intrusive_ptr. If there is need for more advanced behavior,
	// create a new class that conforms to this interface. Intentionally removing virtual
	// to ensure there is no virtual dispatching
	class intrusive_ptr_interface_base
	{
	public:

		int32_t add_ref()
		{
			m_refcount++;
			return m_refcount;
		}

		int32_t release_ref()
		{
			m_refcount--;
			return m_refcount;
		}

		crstl::atomic<int32_t> m_refcount;
	};

	class intrusive_ptr_interface_delete : public intrusive_ptr_interface_base
	{
	public:

		template<typename Q>
		void delete_callback()
		{
			delete (Q*)this;
		}
	};
};