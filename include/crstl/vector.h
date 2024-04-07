#pragma once

#include "crstl/config.h"
#include "crstl/crstldef.h"
#include "crstl/move_forward.h"
#include "crstl/allocator.h"
#include "crstl/compressed_pair.h"
#include "crstl/type_builtins.h"
#include "crstl/utility/placement_new.h"
#include "crstl/utility/memory_ops.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

// crstl::vector
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
// - resize_uninitialized(length): resizes vector to have specified length but does not initialize the contents of the objects. Use with care as
//   objects with assignment operators accessing member variables assumed to be initialized can crash
// - 

crstl_module_export namespace crstl
{
	template<typename T>
	class span;

	template<typename T, typename Allocator = crstl::allocator>
	struct vector_storage
	{
		typedef uint32_t               length_type;

		static const size_t kDataSize = sizeof(T);

		crstl_constexpr vector_storage() crstl_noexcept : m_data(nullptr), m_length(0), m_capacity_allocator() {}

		size_t get_capacity() const
		{
			return m_capacity_allocator.m_first;
		}

		// Reallocate memory if length is the same as capacity. Mainly used for push and emplace functions
		void reallocate_if_length_equals_capacity()
		{
			if (m_length == m_capacity_allocator.m_first)
			{
				reallocate_larger(m_length + 1);
			}
		}

		void reallocate_if_length_greater_than_capacity(size_t length)
		{
			if (length > m_capacity_allocator.m_first)
			{
				reallocate_larger(length);
			}
		}

	protected:

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

			// Destroy existing data
			destruct_or_ignore(m_data, m_length);

			m_capacity_allocator.second().deallocate(m_data, m_capacity_allocator.m_first * kDataSize);
			m_data = temp;
			m_capacity_allocator.m_first = new_capacity;
		}

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

		T* m_data;

		length_type m_length;

		compressed_pair<size_t, Allocator> m_capacity_allocator;
	};

	template<typename T, typename Allocator = crstl::allocator>
	class vector : public vector_base<T, vector_storage<T, Allocator>>
	{
	public:

		typedef vector_base<T, vector_storage<T, Allocator>> base_type;
		typedef vector<T, Allocator>                         this_type;

		typedef typename base_type::length_type     length_type;
		typedef typename base_type::reference       reference;
		typedef typename base_type::const_reference const_reference;
		typedef typename base_type::iterator        iterator;
		typedef typename base_type::const_iterator  const_iterator;
		typedef typename base_type::pointer         pointer;
		typedef typename base_type::const_pointer   const_pointer;

		using base_type::m_length;
		using base_type::m_data;
		using base_type::m_capacity_allocator;

		using base_type::allocate;
		using base_type::deallocate;
		using base_type::reallocate_larger;
		using base_type::reallocate_if_length_equals_capacity;
		using base_type::reallocate_if_length_greater_than_capacity;

		using base_type::back;
		using base_type::clear;
		using base_type::push_back;

		static const size_t kDataSize = sizeof(T);

		crstl_constexpr vector() crstl_noexcept : base_type() {}

		crstl_constexpr14 vector(size_t initialLength)
		{
			m_data = allocate(initialLength);
			default_initialize_or_memset_zero(m_data, initialLength);
			m_length = (length_type)initialLength;
		}

		crstl_constexpr14 vector(size_t initialLength, const T& value)
		{
			m_data = allocate(initialLength);
			set_initialize_or_memset(m_data, value, initialLength);
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

			size_t iter_length = (size_t)(iter2 - iter1);
			m_data = allocate(iter_length);

			copy_initialize_or_memcpy(m_data, iter1, iter_length);
			m_length = (length_type)iter_length;
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		crstl_constexpr14 vector(std::initializer_list<T> ilist) crstl_noexcept : base_type()
		{
			crstl_assert(ilist.end() >= ilist.begin());

			size_t ilist_length = (size_t)(ilist.end() - ilist.begin());

			m_data = allocate(ilist_length);

			for (const T& iter : ilist)
			{
				push_back(iter);
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
			crstl_assert(this != &other);

			// Call destructors for all existing objects
			destruct_or_ignore(m_data, m_length);

			// If we don't have enough capacity, create more space
			if (m_capacity_allocator.m_first < other.m_length)
			{
				// Deallocate the memory
				deallocate();

				// Allocate new memory to hold objects
				m_data = allocate(other.m_length);
			}

			copy_initialize_or_memcpy(m_data, other.m_data, other.m_length);

			m_length = other.m_length;

			return *this;
		}

		crstl_constexpr14 this_type& operator = (this_type&& other) crstl_noexcept
		{
			crstl_assert(this != &other);

			// Swap relevant data
			m_data = other.m_data;
			m_length = other.m_length;
			m_capacity_allocator = other.m_capacity_allocator;

			other.m_data = nullptr;
			other.m_length = 0;

			return *this;
		}

		crstl_constexpr14 void shrink_to_fit()
		{
			if (m_length < m_capacity_allocator.m_first)
			{
				T* temp = (T*)m_capacity_allocator.second().allocate(m_length * kDataSize);
				
				// Copy existing data
				copy_initialize_or_memcpy(temp, m_data, m_length);

				m_capacity_allocator.second().deallocate(m_data, m_capacity_allocator.m_first * kDataSize);
				m_data = temp;
				m_capacity_allocator.m_first = m_length;
			}
		}

		operator span<T>() const;
	};

	template<typename T, typename Allocator>
	vector<T, Allocator>::operator span<T>() const
	{
		return span<T>((T*)m_data, (size_t)m_length);
	}
};