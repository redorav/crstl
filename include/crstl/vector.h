#pragma once

#include "config.h"

#include "config_fwd.h"

#include "type_utils.h"

#include "crstldef.h"

#include "allocator.h"

#include <initializer_list>

// vector
//
// This is a replacement for std::vector
//

crstl_module_export namespace crstl
{
	template<typename T>
	class span;

	template<typename T, typename Allocator = crstl::allocator<T>>
	class vector
	{
	public:

		typedef vector<T, Allocator>   this_type;
		typedef T&                     reference;
		typedef const T&               const_reference;
		typedef T*                     pointer;
		typedef const T*               const_pointer;
		typedef T*                     iterator;
		typedef const T*               const_iterator;
		typedef uint32_t               length_type;

		crstl_constexpr vector() crstl_noexcept : m_data(nullptr), m_length(0), m_capacity(0) {}

		crstl_constexpr vector(size_t initialLength) : m_length(0), m_capacity(initialLength)
		{
			m_data = m_allocator.allocate(initialLength);

			for (size_t i = 0; i < initialLength; ++i)
			{
				push_back();
			}
		}

		crstl_constexpr vector(const this_type& other) crstl_noexcept
		{
			m_data = m_allocator.allocate(other.m_length);
			m_capacity = other.m_length;

			// Copy the incoming objects through their copy constructor
			for (size_t i = 0; i < other.m_length; ++i)
			{
				::new((void*)&m_data[i]) T(other.m_data[i]);
			}

			m_length = other.m_length;
		}

		crstl_constexpr vector(this_type&& other) crstl_noexcept
		{
			m_data = m_allocator.allocate(other.m_length);
			m_capacity = other.m_length;

			// Copy the incoming objects through their copy constructor
			for (size_t i = 0; i < other.m_length; ++i)
			{
				::new((void*)&m_data[i]) T(crstl::move(other.m_data[i]));
			}

			m_length = other.m_length;
		}

		template<typename Iterator>
		crstl_constexpr vector(Iterator iter1, Iterator iter2) crstl_noexcept
		{
			crstl_assert(iter1 != nullptr && iter2 != nullptr);
			crstl_assert(iter2 >= iter1);

			size_t iter_length = iter2 - iter1;
			m_data = m_allocator.allocate(iter_length);

			for (size_t i = 0; i < iter_length; ++i)
			{
				::new((void*)&m_data[i]) T(iter1[i]);
			}

			m_capacity = iter_length;
			m_length = (length_type)iter_length;
		}

		crstl_constexpr vector(std::initializer_list<T> ilist) crstl_noexcept : m_length(0)
		{
			crstl_assert(ilist.end() >= ilist.begin());

			size_t ilist_length = ilist.end() - ilist.begin();

			m_data = m_allocator.allocate(ilist_length);

			for (const T* ptr = ilist.begin(), *end = ilist.end(); ptr != end; ++ptr)
			{
				push_back(*ptr);
			}
		}

		~vector() crstl_noexcept
		{
			clear();

			m_allocator.deallocate(m_data, m_capacity);
		}

		this_type& operator = (const this_type& other) crstl_noexcept
		{
			// Call destructors for all existing objects
			for (size_t i = 0; i < m_length; ++i)
			{
				m_data[i].~T();
			}

			// If we don't have enough capacity, create more space
			if (m_capacity < other.m_length)
			{
				// Deallocate the memory
				m_allocator.deallocate(m_data, m_capacity);

				// Allocate new memory to hold objects
				m_data = m_allocator.allocate(other.m_length);
				m_capacity = other.m_length;
			}

			// Copy the incoming objects through their copy constructor
			for (size_t i = 0; i < other.m_length; ++i)
			{
				::new((void*)&m_data[i]) T(other.m_data[i]);
			}

			m_length = other.m_length;

			return *this;
		}

		reference at(size_t i) { crstl_assert(i < m_length); return m_data[i]; }
		const_reference at(size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		reference back() { crstl_assert(m_length > 0); return m_data[m_length - 1]; }
		const_reference back() const { crstl_assert(m_length > 0); return m_data[m_length - 1]; }

		iterator begin() { return &m_data[0]; }
		const_iterator begin() const { return &m_data[0]; }
		const_iterator cbegin() const { return &m_data[0]; }

		size_t capacity() const { return m_capacity; }

		void clear()
		{
			for (size_t i = 0; i < m_length; ++i)
			{
				m_data[i].~T();
			}

			m_length = 0;
		}

		pointer data() { return &m_data[0]; }
		const_pointer data() const { return &m_data[0]; }

#if defined(CRSTL_VARIADIC_TEMPLATES)
		template<typename... Args>
		reference emplace_back(Args&&... args)
		{
			// todo resize

			::new((void*)&m_data[m_length]) T(crstl::forward<Args>(args)...);
			m_length++;
			return back();
		}
#else
		template<typename Arg0> reference emplace_back(Arg0&& arg0)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1> reference emplace_back(Arg0&& arg0, Arg1&& arg1)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8), crstl::forward<Arg9>(arg9));
			m_length++; return back();
		}

#endif

		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		iterator end() { return &m_data[0] + m_length; }
		const_iterator end() const { return &m_data[0] + m_length; }
		const_iterator cend() const { return &m_data[0] + m_length; }

		reference front() { m_data[0]; }
		const_reference front() const { m_data[0]; }

		void pop_back()
		{
			crstl_assert(m_length > 0);
			back().~T();
			m_length--;
		}

		reference push_back()
		{
			if (m_length == m_capacity)
			{
				reallocate_larger(compute_new_capacity());
			}

			::new((void*)&m_data[m_length]) T();
			m_length++;
			return back();
		}

		void push_back(const T& v)
		{
			if (m_length == m_capacity)
			{
				reallocate_larger(compute_new_capacity());
			}

			::new((void*)&m_data[m_length]) T(v);
			m_length++;
		}

		void push_back(T&& v)
		{
			if (m_length == m_capacity)
			{
				reallocate_larger(compute_new_capacity());
			}

			::new((void*)&m_data[m_length]) T(crstl::move(v));
			m_length++;
		}

		void reserve(size_t length)
		{
			if (length > (size_t)m_length)
			{
				reallocate_larger(length);
			}
		}

		void resize(size_t length)
		{
			if (length > (size_t)m_length)
			{
				reallocate_larger(length);

				for (size_t i = m_length; i < length; ++i)
				{
					::new((void*)&m_data[i]) T();
				}
			}
			else if (length < (size_t)m_length)
			{
				for (size_t i = length; i < m_length; ++i)
				{
					m_data[i].~T();
				}
			}

			m_length = (length_type)length;
		}

		void resize(size_t length, const T& value)
		{
			if (length > (size_t)m_length)
			{
				reallocate_larger(length);

				for (size_t i = m_length; i < length; ++i)
				{
					::new((void*)&m_data[i]) T(value);
				}
			}
			else if (length < (size_t)m_length)
			{
				for (size_t i = length; i < m_length; ++i)
				{
					m_data[i].~T();
				}
			}

			m_length = (length_type)length;
		}

		size_t size() const { return m_length; }

		reference operator [] (size_t i) { crstl_assert(i < m_length); return m_data[i]; }

		const_reference operator [] (size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		operator span<T>() const;

	private:

		size_t compute_new_capacity()
		{
			return 1 + m_capacity * 2;
		}

		// Reallocate vector to a quantity larger than the current one, or the capacity adjusted
		// with the growth factor, whichever is larger
		void reallocate_larger(size_t capacity)
		{
			crstl_assert(capacity > m_capacity);

			size_t growth_capacity = m_capacity * 2; // TODO Growth factor
			size_t new_capacity = capacity > growth_capacity ? capacity : growth_capacity;

			T* temp = m_allocator.allocate(new_capacity);

			// Copy existing data
			for (size_t i = 0; i < m_length; ++i)
			{
				::new((void*)&temp[i]) T(m_data[i]);
			}

			m_allocator.deallocate(m_data, m_capacity);
			m_data = temp;
			m_capacity = new_capacity;
		}

		T* m_data;

		length_type m_length;

		size_t m_capacity;

		Allocator m_allocator;
	};

	template<typename T, typename Allocator>
	vector<T, Allocator>::operator span<T>() const
	{
		return span<T>((T*)m_data, (size_t)m_length);
	}
};