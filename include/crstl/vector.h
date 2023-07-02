#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/move_forward.h"

#include "crstl/crstldef.h"

#include "crstl/allocator.h"

#include "crstl/compressed_pair.h"

#include "crstl/type_builtins.h"

#include "crstl/internal/memory_ops.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_INITIALIZER_LISTS)
#include <initializer_list>
#endif

// vector
//
// Replacement for std::vector
//
// - Guaranteed linear memory
// - No allocation for new, empty vector
// - vector of bool is a vector of bool, not a bitvector
//
// Non-standard functions
// 
// - push_back(): push back an empty default object
// - push_back_uninitialized(): push back an uninitialized object
// - resize_uninitialized(length): resizes vector to have specified length
//   but does not initialize the contents of the objects. Use with care as
//   objects with assignment operators accessing member variables assumed
//   to be initialized can crash
// - 

crstl_module_export namespace crstl
{
	template<typename T>
	class span;

	template<typename T, typename Allocator = crstl::allocator>
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

		static const size_t kDataSize = sizeof(T);

		crstl_constexpr vector() crstl_noexcept : m_data(nullptr), m_length(0), m_capacity_allocator() {}

		crstl_constexpr14 vector(size_t initialLength)
		{
			m_data = allocate(initialLength);

			for (size_t i = 0; i < initialLength; ++i)
			{
				::new((void*)&m_data[i]) T();
			}

			m_length = (length_type)initialLength;
		}

		crstl_constexpr14 vector(size_t initialLength, const T& value)
		{
			m_data = allocate(initialLength);

			for (size_t i = 0; i < initialLength; ++i)
			{
				::new((void*)&m_data[i]) T(value);
			}

			m_length = (length_type)initialLength;
		}

		crstl_constexpr14 vector(const this_type& other) crstl_noexcept
		{
			m_data = allocate(other.m_length);
			copy_initialize_or_memcpy(m_data, other.m_data, other.m_length);
			m_length = other.m_length;
		}

		crstl_constexpr14 vector(this_type&& other) crstl_noexcept
		{
			// Swap relevant data
			m_data = other.m_data;
			m_length = other.m_length;
			m_capacity_allocator = other.m_capacity_allocator;

			other.m_data = nullptr;
			other.m_length = 0;
		}

		crstl_constexpr14 vector(T* iter1, T* iter2) crstl_noexcept
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

#if defined(CRSTL_INITIALIZER_LISTS)

		crstl_constexpr14 vector(std::initializer_list<T> ilist) crstl_noexcept : m_length(0)
		{
			crstl_assert(ilist.end() >= ilist.begin());

			size_t ilist_length = ilist.end() - ilist.begin();

			m_data = allocate(ilist_length);

			for (const T* ptr = ilist.begin(), *end = ilist.end(); ptr != end; ++ptr)
			{
				push_back(*ptr);
			}
		}
#endif

		~vector() crstl_noexcept
		{
			clear();
			deallocate();
		}

		crstl_constexpr14 this_type& operator = (const this_type& other) crstl_noexcept
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

		crstl_constexpr14 reference at(size_t i) { crstl_assert(i < m_length); return m_data[i]; }
		crstl_constexpr const_reference at(size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		crstl_constexpr14 reference back() { crstl_assert(m_length > 0); return m_data[m_length - 1]; }
		crstl_constexpr const_reference back() const { crstl_assert(m_length > 0); return m_data[m_length - 1]; }

		crstl_constexpr14 iterator begin() { return &m_data[0]; }
		crstl_constexpr const_iterator begin() const { return &m_data[0]; }
		crstl_constexpr const_iterator cbegin() const { return &m_data[0]; }

		crstl_constexpr size_t capacity() const { return m_capacity_allocator.m_first; }

		crstl_constexpr14 void clear()
		{
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				for (size_t i = 0; i < m_length; ++i)
				{
					m_data[i].~T();
				}
			}

			m_length = 0;
		}

		crstl_constexpr14 pointer data() { return &m_data[0]; }
		crstl_constexpr const_pointer data() const { return &m_data[0]; }

#if defined(CRSTL_VARIADIC_TEMPLATES)
		template<typename... Args>
		crstl_constexpr14 reference emplace_back(Args&&... args)
		{
			reallocate_if_length_capacity();

			::new((void*)&m_data[m_length]) T(crstl::forward<Args>(args)...);
			m_length++;
			return back();
		}
#else
		template<typename Arg0> reference emplace_back(Arg0&& arg0)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1> reference emplace_back(Arg0&& arg0, Arg1&& arg1)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
		{
			reallocate_if_length_capacity(); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8), crstl::forward<Arg9>(arg9));
			m_length++; return back();
		}

#endif

		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		crstl_constexpr14 iterator end() { return &m_data[0] + m_length; }
		crstl_constexpr const_iterator end() const { return &m_data[0] + m_length; }
		crstl_constexpr const_iterator cend() const { return &m_data[0] + m_length; }

		crstl_constexpr14 reference front() { return m_data[0]; }
		crstl_constexpr const_reference front() const { return m_data[0]; }

		crstl_constexpr14 void pop_back()
		{
			crstl_assert(m_length > 0);
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				back().~T();
			}
			m_length--;
		}

		//----------
		// push_back
		//----------

		crstl_constexpr14 reference push_back()
		{
			reallocate_if_length_capacity();
			::new((void*)&m_data[m_length]) T();
			m_length++;
			return back();
		}

		crstl_constexpr14 void push_back(const T& v)
		{
			reallocate_if_length_capacity();
			::new((void*)&m_data[m_length]) T(v);
			m_length++;
		}

		crstl_constexpr14 void push_back(T&& v)
		{
			reallocate_if_length_capacity();
			::new((void*)&m_data[m_length]) T(crstl::move(v));
			m_length++;
		}

		crstl_constexpr14 reference push_back_uninitialized()
		{
			reallocate_if_length_capacity();
			m_length++;
			return back();
		}

		crstl_constexpr14 void reserve(size_t capacity)
		{
			if (capacity > (size_t)m_capacity_allocator.m_first)
			{
				reallocate_larger(capacity);
			}
		}

		crstl_constexpr14 void resize(size_t length)
		{
			if (length > (size_t)m_length)
			{
				if (length > m_capacity_allocator.m_first)
				{
					reallocate_larger(length);
				}

				for (size_t i = m_length; i < length; ++i)
				{
					::new((void*)&m_data[i]) T();
				}
			}
			else if (length < (size_t)m_length)
			{
				crstl_constexpr_if(!crstl_is_trivially_destructible(T))
				{
					for (size_t i = length; i < m_length; ++i)
					{
						m_data[i].~T();
					}
				}
			}

			m_length = (length_type)length;
		}

		crstl_constexpr14 void resize(size_t length, const T& value)
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
				crstl_constexpr_if(!crstl_is_trivially_destructible(T))
				{
					for (size_t i = length; i < m_length; ++i)
					{
						m_data[i].~T();
					}
				}
			}

			m_length = (length_type)length;
		}

		crstl_constexpr14 void resize_uninitialized(size_t length)
		{
			if (length > (size_t)m_length)
			{
				reallocate_larger(length);
			}
			else if (length < (size_t)m_length)
			{
				crstl_constexpr_if(!crstl_is_trivially_destructible(T))
				{
					for (size_t i = length; i < m_length; ++i)
					{
						m_data[i].~T();
					}
				}
			}

			m_length = (length_type)length;
		}

		crstl_constexpr14 void shrink_to_fit()
		{
			if (m_length < m_capacity_allocator.m_first)
			{
				T* temp = (T*)m_capacity_allocator.second().allocate(m_length * kDataSize);
				
				// Copy existing data
				for (size_t i = 0; i < m_length; ++i)
				{
					::new((void*)&temp[i]) T(m_data[i]);
				}

				m_capacity_allocator.second().deallocate(m_data, m_capacity_allocator.m_first * kDataSize);
				m_data = temp;
				m_capacity_allocator.m_first = m_length;
			}
		}

		crstl_constexpr size_t size() const { return m_length; }

		crstl_constexpr14 reference operator [] (size_t i) { crstl_assert(i < m_length); return m_data[i]; }

		crstl_constexpr const_reference operator [] (size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		//---------------------
		// Comparison Operators
		//---------------------

		crstl_constexpr bool operator == (const this_type& other) const crstl_noexcept
		{
			if (m_length == other.m_length)
			{
				for (size_t i = 0; i < m_length; ++i)
				{
					if (!(m_data[i] == other.m_data[i]))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		crstl_constexpr bool operator != (const this_type& other) const crstl_noexcept
		{
			return !(*this == other);
		}

		operator span<T>() const;

	private:

		crstl_constexpr14 T* allocate(size_t capacity)
		{
			T* temp = (T*)m_capacity_allocator.second().allocate(capacity * kDataSize);
			m_capacity_allocator.m_first = capacity;
			return temp;
		}

		crstl_constexpr14 void deallocate()
		{
			m_capacity_allocator.second().deallocate(m_data, m_capacity_allocator.m_first * kDataSize);
			m_capacity_allocator.m_first = 0;
			m_data = nullptr;
		}

		crstl_constexpr size_t compute_new_capacity(size_t old_capacity) const
		{
			return old_capacity + (old_capacity * 50) / 100;
		}

		// Reallocate memory if length is the same as capacity. Mainly used for
		// push and emplace functions
		crstl_constexpr14 void reallocate_if_length_capacity()
		{
			if (m_length == m_capacity_allocator.m_first)
			{
				reallocate_larger(m_length + 1);
			}
		}

		// Reallocate vector to a quantity larger than the current one, or the capacity adjusted
		// with the growth factor, whichever is larger
		crstl_constexpr14 void reallocate_larger(size_t requested_capacity)
		{
			size_t current_capacity = m_capacity_allocator.m_first;
			size_t growth_capacity = compute_new_capacity(current_capacity);
			size_t new_capacity = requested_capacity > growth_capacity ? requested_capacity : growth_capacity;

			crstl_assert(new_capacity > current_capacity);

			T* temp = (T*)m_capacity_allocator.second().allocate(new_capacity * kDataSize);

			// Copy existing data
			copy_initialize_or_memcpy(temp, m_data, m_length);

			m_capacity_allocator.second().deallocate(m_data, m_capacity_allocator.m_first * kDataSize);
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