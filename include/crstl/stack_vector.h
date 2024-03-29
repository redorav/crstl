#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/move_forward.h"

#include "crstl/crstldef.h"

#include "crstl/utility/memory_ops.h"

// crstl::stack_vector
//
// Replacement for std::vector that doesn't manage its own memory. Useful for using dynamically-size stack-allocated memory via
// the alloca macro, but any other source of externally managed memory works too. stack_vector does not deallocate or free
// memory, but it does call the destructor of objects it created
//
// Remember that alloca has function-scope lifetime, not braced. Also, reserving too much stack memory can cause a stack overflow,
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

#define crstl_alloca_t(T, capacity) crstl::transient_memory_t<T>((T*)crstl_alloca((capacity) * sizeof(T)), (capacity))

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

		crstl_constexpr14 stack_vector(transient_memory_t<T> init)
			: m_data((T*)init.memory)
			, m_capacity(init.capacity)
			, m_length(0) {}
		
		~stack_vector()
		{
			clear();
		}

		crstl_constexpr14 T& at(size_t i) { crstl_assert(i < m_length); return m_data[i]; }
		crstl_constexpr14 const T& at(size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		crstl_constexpr14 T& back() { crstl_assert(m_length > 0); return m_data[m_length - 1]; }
		crstl_constexpr14 const T& back() const { crstl_assert(m_length > 0); return m_data[m_length - 1]; }

		crstl_constexpr14 iterator begin() { return &m_data[0]; }
		crstl_constexpr14 const_iterator begin() const { return &m_data[0]; }
		crstl_constexpr14 const_iterator cbegin() const { return &m_data[0]; }

		crstl_constexpr14 size_t capacity() const { return m_capacity; }

		crstl_constexpr14 void clear()
		{
			destruct_or_ignore(m_data, m_length);
			m_length = 0;
		}

		crstl_constexpr14 pointer data() { return &m_data[0]; }
		crstl_constexpr14 const_pointer data() const { return &m_data[0]; }

#if defined(CRSTL_VARIADIC_TEMPLATES)
		template<typename... Args>
		crstl_constexpr14 T& emplace_back(Args&&... args)
		{
			crstl_assert(m_length < m_capacity);
			crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Args>(args)...);
			m_length++;
			return back();
		}
#else
		template<typename Arg0> T& emplace_back(Arg0&& arg0)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1> T& emplace_back(Arg0&& arg0, Arg1&& arg1)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8));
			m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
		{
			crstl_assert(m_length < m_capacity); crstl_placement_new((void*)&m_data[m_length])
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

		crstl_constexpr14 T& front() { return m_data[0]; }
		crstl_constexpr const T& front() const { return m_data[0]; }

		crstl_constexpr14 void pop_back()
		{
			crstl_assert(m_length > 0);
			destruct_or_ignore(back());
			m_length--;
		}

		crstl_constexpr14 T& push_back()
		{
			crstl_assert(m_length < m_capacity);
			default_initialize_or_memset_zero(m_data[m_length]);
			m_length++;
			return back();
		}

		crstl_constexpr14 void push_back(const T& v)
		{
			crstl_assert(m_length < m_capacity);
			set_initialize_or_memset(m_data[m_length], v);
			m_length++;
		}

		crstl_constexpr14 void push_back(T&& v)
		{
			crstl_assert(m_length < m_capacity);
			crstl_placement_new((void*)&m_data[m_length]) T(crstl::move(v));
			m_length++;
		}

		crstl_constexpr14 size_t size() const { return m_length; }

		crstl_constexpr14 T& operator [] (size_t i) { crstl_assert(i < m_length); return m_data[i]; }

		crstl_constexpr14 const T& operator [] (size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		//---------------------
		// Comparison Operators
		//---------------------

		crstl_constexpr14 bool operator == (const this_type& other) const crstl_noexcept
		{
			if (m_length == other.m_length)
			{
				for (size_t i = 0; i < m_length; ++i)
				{
					if (!(m_data[i] == other.m_data[i])) { return false; }
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		crstl_constexpr14 bool operator != (const this_type& other) const crstl_noexcept
		{
			return !(*this == other);
		}

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
