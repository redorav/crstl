#pragma once

#include "crstl/config.h"
#include "crstl/pair.h"
#include "crstl/hash.h"
#include "crstl/type_array.h"
#include "crstl/allocator.h"
#include "crstl/compressed_pair.h"

#include "crstl/utility/memory_ops.h"
#include "crstl/utility/hashmap_common.h"

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

		typedef Key                                  key_type;
		typedef T                                    mapped_type;
		typedef pair<Key, T>                         key_value_type;
		typedef size_t                               size_type;
		typedef Hasher                               hasher;
		typedef open_iterator<key_value_type, false> iterator;
		typedef open_iterator<key_value_type, true>  const_iterator;
		typedef open_node<key_value_type>            node_type;

		static const size_t kNodeSize = sizeof(node_type);

		crstl_constexpr14 open_hashmap_storage() crstl_noexcept : m_data(nullptr), m_length(0), m_capacity_allocator() {}

		// Assume our buckets are power of 2
		size_t compute_bucket(size_t hash_value) const
		{
			return compute_bucket_function<true>::compute_bucket(hash_value, m_capacity_allocator.m_first);
		}

		size_t get_capacity() const
		{
			return m_capacity_allocator.m_first;
		}

		template<typename RehashFunction>
		crstl_forceinline crstl_constexpr14 void reallocate_rehash_if_length_above_capacity(size_t length, RehashFunction rehash_function)
		{
			if (length > m_capacity_allocator.m_first)
			{
				node_type* current_data = m_data;
				size_t current_capacity = get_capacity();
				size_t new_capacity = length;

				m_data = nullptr;
				m_length = 0;
				allocate_internal(new_capacity);

				for (size_t i = 0; i < new_capacity; ++i)
				{
					m_data[i].set_empty();
				}

				rehash_function(current_data, current_data + current_capacity);

				deallocate(current_data, current_capacity);
			}
		}

		template<typename RehashFunction>
		crstl_forceinline crstl_constexpr14 void reallocate_rehash_if_length_above_threshold(RehashFunction rehash_function)
		{
			// length * 0.75
			size_t length_threshold = (m_capacity_allocator.m_first >> 1) + (m_capacity_allocator.m_first >> 2);

			if (m_length >= length_threshold)
			{
				node_type* current_data = m_data;
				size_t current_capacity = get_capacity();
				size_t new_capacity = compute_new_capacity(current_capacity);

				m_data = nullptr;
				m_length = 0;
				allocate_internal(new_capacity);

				for (size_t i = 0; i < new_capacity; ++i)
				{
					m_data[i].set_empty();
				}

				rehash_function(current_data, current_data + current_capacity);

				deallocate(current_data, current_capacity);
			}
		}

		static inline size_t align(size_t x, size_t alignment) crstl_noexcept
		{
			return (x + (alignment - 1)) & ~(alignment - 1);
		}

		node_type* allocate(size_t capacity)
		{
			size_t size_bytes = capacity * kNodeSize;
			size_t alignment = alignof(node_type) > 64 ? alignof(node_type) : 64;
			size_bytes = align(size_bytes, alignment);

			return (node_type*)m_capacity_allocator.second().allocate(capacity * kNodeSize);
		}

		void deallocate(node_type* data, size_t capacity)
		{
			m_capacity_allocator.second().deallocate(data, capacity * kNodeSize);
		}

		crstl_constexpr14 void allocate_internal(size_t capacity)
		{
			crstl_assert(m_data == nullptr);
			m_data = allocate(capacity);
			m_capacity_allocator.m_first = capacity;
		}
		
		crstl_constexpr14 void deallocate_internal()
		{
			deallocate(m_data, m_capacity_allocator.m_first);
			m_capacity_allocator.m_first = 0;
			m_data = nullptr;
		}

		crstl_constexpr size_t compute_new_capacity(size_t old_capacity) const
		{
			size_t new_capacity = 2 * old_capacity;

			if (new_capacity < 16)
			{
				new_capacity = 16;
			}

			return new_capacity;
		}

		node_type* m_data;

		size_t m_length;

		compressed_pair<size_t, Allocator> m_capacity_allocator;
	};

	template<typename Key, typename T, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator>
	class open_hashmap : public open_hashmap_base<open_hashmap_storage<Key, T, Hasher, Allocator>>
	{
	public:

		typedef open_hashmap_base<open_hashmap_storage<Key, T, Hasher, Allocator>> base_type;
		typedef open_hashmap                                                       this_type;

		typedef typename base_type::key_type       key_type;
		typedef typename base_type::mapped_type    mapped_type;
		typedef typename base_type::key_value_type key_value_type;
		typedef typename base_type::size_type      size_type;
		typedef typename base_type::iterator       iterator;
		typedef typename base_type::const_iterator const_iterator;
		typedef typename base_type::node_type      node_type;

		using base_type::clear;
		using base_type::insert;

		crstl_constexpr14 open_hashmap() crstl_noexcept : base_type() {}

		crstl_constexpr14 open_hashmap(size_t initial_length) crstl_noexcept
		{
			allocate_internal(initial_length);

			for (size_t i = 0; i < initial_length; ++i)
			{
				m_data[i].set_empty();
			}
		}

		crstl_constexpr14 open_hashmap(const open_hashmap& other) crstl_noexcept : open_hashmap(other.m_length)
		{
			for (const key_value_type& iter : other)
			{
				insert(iter);
			}
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		crstl_constexpr14 open_hashmap(std::initializer_list<key_value_type> ilist) crstl_noexcept : open_hashmap((size_t)ilist.size())
		{
			for (const key_value_type& iter : ilist)
			{
				insert(iter);
			}
		}

#endif

		~open_hashmap() crstl_noexcept
		{
			destructor();
		}

		crstl_constexpr14 open_hashmap& operator = (const open_hashmap& other)
		{
			crstl_assert(this != &other);

			clear();

			for (const key_value_type& iter : other)
			{
				insert(iter);
			}

			return *this;
		}

		crstl_constexpr14 open_hashmap& operator = (open_hashmap&& other)
		{
			crstl_assert(this != &other);

			destructor();

			m_data = other.m_data;
			m_length = other.m_length;
			m_capacity_allocator = other.m_capacity_allocator;

			other.m_data = nullptr;
			other.m_length = 0;
			other.m_capacity_allocator.m_first = 0;

			return *this;
		}

		static void swap(this_type& hashmap1, this_type& hashmap2)
		{
			node_type* data = hashmap2.m_data;
			size_t length = hashmap2.m_length;
			compressed_pair<size_t, Allocator> capacity_allocator = hashmap2.m_capacity_allocator;

			hashmap2.m_data = hashmap1.m_data;
			hashmap2.m_length = hashmap1.m_length;
			hashmap2.m_capacity_allocator = hashmap1.m_capacity_allocator;

			hashmap1.m_data = data;
			hashmap1.m_length = length;
			hashmap1.m_capacity_allocator = capacity_allocator;
		}

	private:

		void destructor()
		{
			// Only destroy the value, no need to destroy buckets or nodes
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				for (const key_value_type& iter : *this)
				{
					iter.second.~T();
				}
			}

			deallocate_internal();
		}

		using base_type::allocate_internal;
		using base_type::deallocate_internal;

		using base_type::m_data;
		using base_type::m_length;
		using base_type::m_capacity_allocator;
	};
};