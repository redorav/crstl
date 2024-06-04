#pragma once

#include "crstl/open_hashtable_base.h"

#include "crstl/allocator.h"
#include "crstl/compressed_pair.h"
#include "crstl/bit.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

crstl_module_export namespace crstl
{
	template<typename Key, typename T, typename Hasher, typename Allocator>
	class open_hashmap_storage
	{
	protected:

		typedef Key                                                  key_type;
		typedef T                                                    value_type;
		typedef size_t                                               size_type;
		typedef Hasher                                               hasher;
		typedef open_iterator<key_type, value_type, false>           iterator;
		typedef open_iterator<key_type, value_type, true>            const_iterator;
		typedef open_node<key_type, value_type>                      node_type;
		typedef decltype(open_node<key_type, value_type>::key_value) key_value_type;

		static const size_t kNodeSize = sizeof(node_type);

		crstl_constexpr14 open_hashmap_storage() crstl_noexcept
			: m_data(&m_dummy)
			, m_length(0)
			, m_bucket_count(1) // Need this to work with m_dummy
			, m_capacity_allocator()
		{
			m_dummy.set_empty();
		}

		~open_hashmap_storage() {}

		// Assume our buckets are power of 2
		size_t compute_bucket(size_t hash_value) const
		{
			crstl_assert(crstl::is_pow2(m_bucket_count));
			return compute_bucket_function<true>::compute_bucket(hash_value, m_bucket_count);
		}

		size_t get_bucket_count() const
		{
			return m_bucket_count;
		}

		template<typename RehashFunction>
		crstl_forceinline crstl_constexpr14 void reallocate_rehash_if_length_above_capacity(size_t length, RehashFunction rehash_function)
		{
			if (length > m_capacity_allocator.m_first)
			{
				reallocate_rehash(length, rehash_function);
			}
		}

		template<typename RehashFunction>
		crstl_forceinline crstl_constexpr14 void reallocate_rehash_if_length_above_load_factor(RehashFunction rehash_function)
		{
			// length * 0.75
			size_t length_threshold = (m_capacity_allocator.m_first >> 1) + (m_capacity_allocator.m_first >> 2);

			if (m_length >= length_threshold)
			{
				size_t current_capacity = get_bucket_count();
				size_t new_capacity = compute_new_capacity(current_capacity);
				reallocate_rehash(new_capacity, rehash_function);
			}
		}

		template<typename RehashFunction>
		crstl_forceinline crstl_constexpr14 void reallocate_rehash(size_t new_capacity, RehashFunction rehash_function)
		{
			node_type* current_data = m_data;
			size_t current_capacity = get_bucket_count();

			size_t allocated_capacity = allocate_internal(new_capacity);

			for (size_t i = 0; i < allocated_capacity; ++i)
			{
				m_data[i].set_empty();
			}

			m_length = 0;
			rehash_function(current_data, current_data + current_capacity);

			deallocate(current_data, current_capacity);
		}

		static inline size_t align(size_t x, size_t alignment) crstl_noexcept
		{
			return (x + (alignment - 1)) & ~(alignment - 1);
		}

		// TODO Allocate aligned
		node_type* allocate(size_t capacity)
		{
			return (node_type*)m_capacity_allocator.second().allocate(capacity * kNodeSize);
		}

		void deallocate(node_type* data, size_t capacity)
		{
			if (data != &m_dummy)
			{
				m_capacity_allocator.second().deallocate(data, capacity * kNodeSize);
			}
		}

		crstl_nodiscard
		crstl_constexpr14 size_t allocate_internal(size_t capacity)
		{
			// Round to the nearest power of 2 as we rely on this for the bucket calculation
			size_t rounded_capacity = crstl::bit_ceil(capacity);
			m_data = allocate(rounded_capacity);
			m_capacity_allocator.m_first = rounded_capacity;
			m_bucket_count = rounded_capacity;
			return rounded_capacity;
		}
		
		crstl_constexpr14 void deallocate_internal()
		{
			deallocate(m_data, m_capacity_allocator.m_first);
			m_capacity_allocator.m_first = 0;
			m_data = &m_dummy;
			m_bucket_count = 1;
		}

		crstl_constexpr14 size_t compute_new_capacity(size_t old_capacity) const
		{
			return 2 * old_capacity < 16 ? 16 : 2 * old_capacity;
		}

		node_type* m_data;

		// Use this dummy value to avoid having to check for m_data == nullptr during find and erase
		// We just initialize this to be an always-empty node
		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { node_type m_dummy; };
		};
		crstl_warning_anonymous_struct_union_end

		size_t m_length;

		size_t m_bucket_count;

		compressed_pair<size_t, Allocator> m_capacity_allocator;
	};

	template<typename Key, typename T, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator, bool IsMultipleValue = false>
	class open_hashtable : public open_hashtable_base<open_hashmap_storage<Key, T, Hasher, Allocator>, IsMultipleValue>
	{
	public:

		typedef open_hashtable_base<open_hashmap_storage<Key, T, Hasher, Allocator>, IsMultipleValue> base_type;
		typedef open_hashtable                                                                        this_type;

		typedef typename base_type::key_type       key_type;
		typedef typename base_type::value_type     value_type;
		typedef typename base_type::key_value_type key_value_type;
		typedef typename base_type::size_type      size_type;
		typedef typename base_type::iterator       iterator;
		typedef typename base_type::const_iterator const_iterator;
		typedef typename base_type::node_type      node_type;

		using base_type::clear;

		crstl_constexpr14 open_hashtable() crstl_noexcept : base_type() {}

		crstl_constexpr14 open_hashtable(size_t initial_length) crstl_noexcept
		{
			size_t allocated_length = allocate_internal(initial_length);

			for (size_t i = 0; i < allocated_length; ++i)
			{
				m_data[i].set_empty();
			}
		}

		crstl_constexpr14 open_hashtable(const open_hashtable& other) crstl_noexcept : open_hashtable(other.m_length)
		{
			for (const key_value_type& iter : other)
			{
				insert_empty_impl(iter);
			}
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		crstl_constexpr14 open_hashtable(std::initializer_list<key_value_type> ilist) crstl_noexcept : open_hashtable((size_t)ilist.size())
		{
			for (const key_value_type& iter : ilist)
			{
				insert_empty_impl(iter);
			}
		}

#endif

		~open_hashtable() crstl_noexcept
		{
			destructor();
		}

		crstl_constexpr14 open_hashtable& operator = (const open_hashtable& other)
		{
			crstl_assert(this != &other);

			// Reuse existing allocation if the size fits, just need to destroy existing elements and reset
			if (m_bucket_count >= other.m_length)
			{
				clear();
			}
			// Otherwise destroy elements, deallocate memory and allocate a new piece of memory for the incoming data
			else
			{
				destructor();

				size_t allocated_length = allocate_internal(other.m_length);

				for (size_t i = 0; i < allocated_length; ++i)
				{
					m_data[i].set_empty();
				}
			}

			for (const key_value_type& iter : other)
			{
				insert_empty_impl(iter);
			}

			return *this;
		}

		crstl_constexpr14 open_hashtable& operator = (open_hashtable&& other)
		{
			crstl_assert(this != &other);

			destructor();

			m_data = other.m_data == &other.m_dummy ? &m_dummy : other.m_data;
			m_length = other.m_length;
			m_capacity_allocator = other.m_capacity_allocator;
			m_bucket_count = other.m_bucket_count;

			other.m_data = &other.m_dummy;
			other.m_length = 0;
			other.m_capacity_allocator.m_first = 0;
			other.m_bucket_count = 1;

			return *this;
		}

		static void swap(open_hashtable& hashmap1, open_hashtable& hashmap2)
		{
			node_type* data = hashmap2.m_data == &hashmap2.m_dummy ? &hashmap1.m_dummy : hashmap2.m_data;
			size_t length = hashmap2.m_length;
			compressed_pair<size_t, Allocator> capacity_allocator = hashmap2.m_capacity_allocator;
			size_t bucket_count = hashmap2.m_bucket_count;

			hashmap2.m_data = hashmap1.m_data == &hashmap1.m_dummy ? &hashmap2.m_dummy : hashmap1.m_data;
			hashmap2.m_length = hashmap1.m_length;
			hashmap2.m_capacity_allocator = hashmap1.m_capacity_allocator;
			hashmap2.m_bucket_count = hashmap1.m_bucket_count;

			hashmap1.m_data = data;
			hashmap1.m_length = length;
			hashmap1.m_capacity_allocator = capacity_allocator;
			hashmap1.m_bucket_count = bucket_count;
		}

	private:

		void destructor()
		{
			// Only destroy the value, no need to destroy buckets or nodes
			crstl_constexpr_if(!crstl_is_trivially_destructible(key_value_type))
			{
				for (const key_value_type& iter : *this)
				{
					iter.~key_value_type();
				}
			}

			deallocate_internal();
		}

		using base_type::allocate_internal;
		using base_type::deallocate_internal;
		using base_type::insert_empty_impl;

		using base_type::m_data;
		using base_type::m_dummy;
		using base_type::m_length;
		using base_type::m_capacity_allocator;
		using base_type::m_bucket_count;
	};

	template<typename Key, typename T, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator>
	class open_hashmap : public open_hashtable<Key, T, Hasher, Allocator>
	{
		using open_hashtable<Key, T, Hasher, Allocator>::open_hashtable;
	};

	template<typename Key, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator>
	class open_hashset : public open_hashtable<Key, void, Hasher, Allocator>
	{
		using open_hashtable<Key, void, Hasher, Allocator>::open_hashtable;

	private:

		using open_hashtable<Key, void, Hasher, Allocator>::for_each;
	};

	template<typename Key, typename T, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator>
	class open_multi_hashmap : public open_hashtable<Key, T, Hasher, Allocator, true>
	{
		using open_hashtable<Key, T, Hasher, Allocator, true>::open_hashtable;

	};

	template<typename Key, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator>
	class open_multi_hashset : public open_hashtable<Key, void, Hasher, Allocator, true>
	{
		using open_hashtable<Key, void, Hasher, Allocator, true>::open_hashtable;

	private:

		using open_hashtable<Key, void, Hasher, Allocator, true>::for_each;
	};
};