#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/move_forward.h"

#include "crstl/crstldef.h"

// stack_vector
//
// Replacement for std::vector that doesn't manage its own memory.
// Useful for using dynamically-size stack-allocated memory via
// the alloca macro, but any other source of externally managed 
// memory works too. stack_vector does not deallocate or free
// memory.
//
// Remember that alloca has function-scope lifetime, not braced!
// Also, reserving too much stack memory can cause a stack overflow,
// so use with care.
// 
// - Example usage with stack memory
//   crstl::stack_vector<MyClass> stackvector(crstl_alloca_t(MyClass, 64));
// 
// - default initialization, copy, move and assignment are disallowed
//   to prevent lifetime issues
// 
// - Cannot resize, reserve or otherwise alter capacity once created
// 
// - Consider using a fixed_vector first, and if requirements require dynamic
//   size for temporary usage, use stack_vector instead

#if !defined(alloca)
#define alloca _alloca
#endif

#define crstl_alloca_t(T, capacity) crstl::transient_memory_t<T>((T*)alloca(capacity * sizeof(T)), capacity)

crstl_module_export namespace crstl
{
	template<typename T>
	struct transient_memory_t
	{
		transient_memory_t(T* memory, size_t capacity) : memory(memory), capacity(capacity) {}
		T* memory;
		size_t capacity;
	};

	template<typename T>
	class stack_vector
	{
	public:

		typedef stack_vector<T> this_type;
		typedef uint32_t        length_type;
		typedef T&              reference;
		typedef const T&        const_reference;
		typedef T*              pointer;
		typedef const T*        const_pointer;
		typedef T*              iterator;
		typedef const T*        const_iterator;

		stack_vector(transient_memory_t<T> init)
			: m_data((T*)init.memory)
			, m_capacity(init.capacity)
			, m_length(0) {}

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
			crstl_assert(m_length < m_capacity);
			::new((void*)&m_data[m_length]) T(crstl::forward<Args>(args)...);
			m_length++;
			return back();
		}
#else
		template<typename Arg0> reference emplace_back(Arg0&& arg0)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1> reference emplace_back(Arg0&& arg0, Arg1&& arg1)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
		{
			crstl_assert(m_length < m_capacity); ::new((void*)&m_data[m_length])
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
			crstl_assert(m_length < m_capacity);
			::new((void*)&m_data[m_length]) T();
			m_length++;
			return back();
		}

		void push_back(const T& v)
		{
			crstl_assert(m_length < m_capacity);
			::new((void*)&m_data[m_length]) T(v);
			m_length++;
		}

		void push_back(T&& v)
		{
			crstl_assert(m_length < m_capacity);
			::new((void*)&m_data[m_length]) T(crstl::move(v));
			m_length++;
		}

		size_t size() const { return m_length; }

		reference operator [] (size_t i) { crstl_assert(i < m_length); return m_data[i]; }

		const_reference operator [] (size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

	private:

		// Stack vectors are meant to be transient. They are allocated on the stack and given stack
		// memory for their operations. Because of that, we don't allow default construction or copy
		stack_vector() crstl_constructor_delete;

		stack_vector(const stack_vector& other) crstl_constructor_delete;

		stack_vector(const stack_vector&& other) crstl_constructor_delete;

		T* m_data;

		size_t m_capacity;

		size_t m_length;
	};
};