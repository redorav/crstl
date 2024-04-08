#pragma once

#include "crstl/config.h"
#include "crstl/crstldef.h"
#include "crstl/vector_base.h"

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
	class stack_vector_storage
	{
	public:

		typedef size_t length_type;

		stack_vector_storage() : m_data(nullptr), m_length(0), m_capacity(0) {}

		size_t get_capacity() const { return m_capacity; }

		void reallocate_if_length_equals_capacity()
		{
			crstl_assert(m_length < m_capacity);
		}

		void reallocate_if_length_greater_than_capacity(size_t length)
		{
			crstl_unused(length);
			crstl_assert(length <= m_capacity);
		}

	protected:

		T* m_data;

		length_type m_length;

		size_t m_capacity;
	};

	template<typename T>
	class stack_vector : public vector_base<T, stack_vector_storage<T>>
	{
	public:

		typedef vector_base<T, stack_vector_storage<T>> base_type;
		typedef stack_vector                            this_type;

		typedef typename base_type::length_type     length_type;
		typedef typename base_type::reference       reference;
		typedef typename base_type::const_reference const_reference;
		typedef typename base_type::iterator        iterator;
		typedef typename base_type::const_iterator  const_iterator;
		typedef typename base_type::pointer         pointer;
		typedef typename base_type::const_pointer   const_pointer;

		using base_type::clear;

		crstl_constexpr14 stack_vector(transient_memory_t<T> init)
		{
			m_data = (T*)init.memory;
			m_length = 0;
			m_capacity = init.capacity;
		}
		
		~stack_vector()
		{
			clear();
		}

	private:

		using base_type::resize;
		using base_type::reserve;

		using base_type::m_length;
		using base_type::m_data;
		using base_type::m_capacity;

		// Stack vectors are meant to be transient. They are allocated on the stack and given stack
		// memory for their operations. Because of that, we don't allow default construction or copy
		stack_vector() crstl_constructor_delete;

		stack_vector(const stack_vector& other) crstl_constructor_delete;

		stack_vector(const stack_vector&& other) crstl_constructor_delete;
	};
};
