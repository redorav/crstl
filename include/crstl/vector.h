#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/type_utils.h"

#include "crstl/crstldef.h"

#include "crstl/allocator.h"

#include "crstl/compressed_pair.h"

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

		crstl_constexpr vector() crstl_noexcept : m_data(nullptr), m_length(0)
		{
			m_capacity_allocator.m_first = 0;
		}

		crstl_constexpr vector(size_t initialLength) : m_length(0)
		{
			m_data = allocate(initialLength);

			for (size_t i = 0; i < initialLength; ++i)
			{
				push_back();
			}
		}

		crstl_constexpr vector(const this_type& other) crstl_noexcept
		{
			m_data = allocate(other.m_length);

			// Copy the incoming objects through their copy constructor
			for (size_t i = 0; i < other.m_length; ++i)
			{
				::new((void*)&m_data[i]) T(other.m_data[i]);
			}

			m_length = other.m_length;
		}

		crstl_constexpr vector(this_type&& other) crstl_noexcept
		{
			m_data = allocate(other.m_length);

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
			m_data = allocate(iter_length);

			for (size_t i = 0; i < iter_length; ++i)
			{
				::new((void*)&m_data[i]) T(iter1[i]);
			}

			m_length = (length_type)iter_length;
		}

		crstl_constexpr vector(std::initializer_list<T> ilist) crstl_noexcept : m_length(0)
		{
			crstl_assert(ilist.end() >= ilist.begin());

			size_t ilist_length = ilist.end() - ilist.begin();

			m_data = allocate(ilist_length);

			for (const T* ptr = ilist.begin(), *end = ilist.end(); ptr != end; ++ptr)
			{
				push_back(*ptr);
			}
		}

		~vector() crstl_noexcept
		{
			clear();
			deallocate();
		}

		this_type& operator = (const this_type& other) crstl_noexcept
		{
			// Call destructors for all existing objects
			for (size_t i = 0; i < m_length; ++i)
			{
				m_data[i].~T();
			}

			// If we don't have enough capacity, create more space
			if (m_capacity_allocator.m_first < other.m_length)
			{
				// Deallocate the memory
				deallocate();

				// Allocate new memory to hold objects
				m_data = allocate(other.m_length);
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

		size_t capacity() const { return m_capacity_allocator.m_first; }

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
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); }

			::new((void*)&m_data[m_length]) T(crstl::forward<Args>(args)...);
			m_length++;
			return back();
		}
#else
		template<typename Arg0> reference emplace_back(Arg0&& arg0)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1> reference emplace_back(Arg0&& arg0, Arg1&& arg1)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); } ::new((void*)&m_data[m_length])
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
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); }

			::new((void*)&m_data[m_length]) T();
			m_length++;
			return back();
		}

		void push_back(const T& v)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); }

			::new((void*)&m_data[m_length]) T(v);
			m_length++;
		}

		void push_back(T&& v)
		{
			if (m_length == m_capacity_allocator.m_first) { reallocate_larger(compute_new_capacity()); }

			::new((void*)&m_data[m_length]) T(crstl::move(v));
			m_length++;
		}

		void reserve(size_t capacity)
		{
			if (capacity > (size_t)m_capacity_allocator.m_first)
			{
				reallocate_larger(capacity);
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

		T* allocate(size_t capacity)
		{
			T* temp = m_capacity_allocator.second().allocate(capacity);
			m_capacity_allocator.m_first = capacity;
			return temp;
		}

		void deallocate()
		{
			m_capacity_allocator.second().deallocate(m_data, m_capacity_allocator.m_first);
			m_capacity_allocator.m_first = 0;
			m_data = nullptr;
		}

		size_t compute_new_capacity()
		{
			return 1 + m_capacity_allocator.m_first * 2;
		}

		// Reallocate vector to a quantity larger than the current one, or the capacity adjusted
		// with the growth factor, whichever is larger
		void reallocate_larger(size_t capacity)
		{
			crstl_assert(capacity > m_capacity_allocator.m_first);

			size_t growth_capacity = m_capacity_allocator.m_first * 2; // TODO Growth factor
			size_t new_capacity = capacity > growth_capacity ? capacity : growth_capacity;

			T* temp = m_capacity_allocator.second().allocate(new_capacity);

			// Copy existing data
			for (size_t i = 0; i < m_length; ++i)
			{
				::new((void*)&temp[i]) T(m_data[i]);
			}

			m_capacity_allocator.second().deallocate(m_data, m_capacity_allocator.m_first);
			m_data = temp;
			m_capacity_allocator.m_first = new_capacity;
		}

		T* m_data;

		length_type m_length;

		compressed_pair<size_t, Allocator> m_capacity_allocator;
	};

	template<typename T, typename Allocator>
	vector<T, Allocator>::operator span<T>() const
	{
		return span<T>((T*)m_data, (size_t)m_length);
	}
};