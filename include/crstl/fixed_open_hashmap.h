#pragma once

#include "crstl/open_hashmap_base.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

crstl_module_export namespace crstl
{
	template<typename Key, typename T, size_t NodeCount, typename Hasher>
	class fixed_open_hashmap_storage
	{
	public:

		typedef Key                                  key_type;
		typedef T                                    value_type;
		typedef pair<Key, T>                         key_value_type;
		typedef size_t                               size_type;
		typedef Hasher                               hasher;
		typedef open_iterator<key_value_type, false> iterator;
		typedef open_iterator<key_value_type, true>  const_iterator;
		typedef open_node<key_value_type>            node_type;

		fixed_open_hashmap_storage() : m_length(0) {}

		~fixed_open_hashmap_storage() {}

		template<typename HashmapType>
		crstl_constexpr14 void reallocate_rehash_if_length_above_load_factor(HashmapType)
		{
			crstl_assert(m_length <= NodeCount);
		}

		template<typename HashmapType>
		crstl_constexpr14 void reallocate_rehash_if_length_above_capacity(size_t capacity, HashmapType)
		{
			crstl_unused(capacity);
			crstl_assert(capacity <= NodeCount);
		}

		crstl_constexpr14 size_t compute_bucket(size_t hash_value) const
		{
			return crstl::compute_bucket<NodeCount>(hash_value);
		}

		crstl_constexpr14 size_t get_bucket_count() const { return NodeCount; }

	protected:

		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { node_type m_data[NodeCount]; };
		};
		crstl_warning_anonymous_struct_union_end

		size_t m_length;
	};

	// It is recommended to use a power of 2 for buckets as it is faster to find
	template
	<
		typename Key,
		typename T,
		size_t NodeCount,
		typename Hasher = crstl::hash<Key>
	>
	class fixed_open_hashmap : public open_hashmap_base<fixed_open_hashmap_storage<Key, T, NodeCount, Hasher>>
	{
	public:

		static_assert(NodeCount >= 1, "Must have at least one node");

		typedef open_hashmap_base<fixed_open_hashmap_storage<Key, T, NodeCount, Hasher>> base_type;

		typedef typename base_type::key_type       key_type;
		typedef typename base_type::value_type     value_type;
		typedef typename base_type::key_value_type key_value_type;
		typedef typename base_type::size_type      size_type;
		typedef typename base_type::iterator       iterator;
		typedef typename base_type::const_iterator const_iterator;
		typedef typename base_type::node_type      node_type;

		using base_type::m_data;
		using base_type::m_length;

		using base_type::clear;
		using base_type::insert;

		crstl_constexpr14 fixed_open_hashmap() crstl_noexcept : base_type()
		{
			for (size_t i = 0; i < NodeCount; ++i)
			{
				m_data[i].set_empty();
			}
		}

		crstl_constexpr14 fixed_open_hashmap(const fixed_open_hashmap& other) crstl_noexcept : fixed_open_hashmap()
		{
			for (const key_value_type& iter : other)
			{
				insert(iter);
			}
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		crstl_constexpr14 fixed_open_hashmap(std::initializer_list<key_value_type> ilist) crstl_noexcept : fixed_open_hashmap()
		{
			crstl_assert(ilist.size() <= NodeCount);

			for (const key_value_type& iter : ilist)
			{
				insert(iter);
			}
		}

#endif

		~fixed_open_hashmap() crstl_noexcept
		{
			// Only destroy the value, no need to destroy buckets or nodes
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				for (const key_value_type& iter : *this)
				{
					iter.second.~T();
				}
			}
		}

		crstl_constexpr14 fixed_open_hashmap& operator = (const fixed_open_hashmap& other)
		{
			clear();
		
			for (const key_value_type& iter : other)
			{
				insert(iter);
			}
		
			return *this;
		}
	};
};