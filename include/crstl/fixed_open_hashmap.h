#pragma once

#include "crstl/open_hashtable_base.h"

#include "crstl/forward_declarations.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

crstl_module_export namespace crstl
{
	template<typename Key, typename T, size_t NodeCount, typename Hasher>
	class fixed_open_hashtable_storage
	{
	public:

		typedef Key                                                  key_type;
		typedef T                                                    value_type;
		typedef size_t                                               size_type;
		typedef Hasher                                               hasher;
		typedef open_iterator<key_type, value_type, false>           iterator;
		typedef open_iterator<key_type, value_type, true>            const_iterator;
		typedef open_node<key_type, value_type>                      node_type;
		typedef decltype(open_node<key_type, value_type>::key_value) key_value_type;

		fixed_open_hashtable_storage() : m_length(0) {}

		~fixed_open_hashtable_storage() {}

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
	template<typename Key, typename T, size_t NodeCount, typename Hasher, bool IsMultipleValue>
	class fixed_open_hashtable : public open_hashtable_base<fixed_open_hashtable_storage<Key, T, NodeCount, Hasher>, IsMultipleValue>
	{
	public:

		static_assert(NodeCount >= 1, "Must have at least one node");

		typedef open_hashtable_base<fixed_open_hashtable_storage<Key, T, NodeCount, Hasher>> base_type;

		typedef typename base_type::key_type       key_type;
		typedef typename base_type::value_type     value_type;
		typedef typename base_type::key_value_type key_value_type;
		typedef typename base_type::size_type      size_type;
		typedef typename base_type::iterator       iterator;
		typedef typename base_type::const_iterator const_iterator;
		typedef typename base_type::node_type      node_type;

		using base_type::clear;

		crstl_constexpr14 fixed_open_hashtable() crstl_noexcept : base_type()
		{
			for (size_t i = 0; i < NodeCount; ++i)
			{
				m_data[i].set_empty();
			}
		}

		crstl_constexpr14 fixed_open_hashtable(const fixed_open_hashtable& other) crstl_noexcept : fixed_open_hashtable()
		{
			for (const key_value_type& iter : other)
			{
				insert_empty_impl(iter);
			}
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		crstl_constexpr14 fixed_open_hashtable(std::initializer_list<key_value_type> ilist) crstl_noexcept : fixed_open_hashtable()
		{
			crstl_assert(ilist.size() <= NodeCount);

			for (const key_value_type& iter : ilist)
			{
				insert_empty_impl(iter);
			}
		}

#endif

		~fixed_open_hashtable() crstl_noexcept
		{
			// Only destroy the value, no need to destroy buckets or nodes
			crstl_constexpr_if(!crstl_is_trivially_destructible(key_value_type))
			{
				for (const key_value_type& iter : *this)
				{
					iter.~key_value_type();
				}
			}
		}

		crstl_constexpr14 fixed_open_hashtable& operator = (const fixed_open_hashtable& other)
		{
			clear();
		
			for (const key_value_type& iter : other)
			{
				insert_empty_impl(iter);
			}
		
			return *this;
		}

	private:

		using base_type::insert_empty_impl;

		using base_type::m_data;
		using base_type::m_length;
	};

	template<typename Key, typename T, size_t NodeCount, typename Hasher>
	class fixed_open_hashmap : public fixed_open_hashtable<Key, T, NodeCount, Hasher, false>
	{
		using fixed_open_hashtable<Key, T, NodeCount, Hasher, false>::fixed_open_hashtable;
	};

	template<typename Key, size_t NodeCount, typename Hasher>
	class fixed_open_hashset : public fixed_open_hashtable<Key, void, NodeCount, Hasher, false>
	{
		using fixed_open_hashtable<Key, void, NodeCount, Hasher, false>::fixed_open_hashtable;

	private:

		using fixed_open_hashtable<Key, void, NodeCount, Hasher, false>::for_each;
	};

	template<typename Key, typename T, size_t NodeCount, typename Hasher>
	class fixed_open_multi_hashmap : public fixed_open_hashtable<Key, T, NodeCount, Hasher, true>
	{
		using fixed_open_hashtable<Key, T, NodeCount, Hasher, true>::fixed_open_hashtable;
	};

	template<typename Key, size_t NodeCount, typename Hasher>
	class fixed_open_multi_hashset : public fixed_open_hashtable<Key, void, NodeCount, Hasher, true>
	{
		using fixed_open_hashtable<Key, void, NodeCount, Hasher, true>::fixed_open_hashtable;

	private:

		using fixed_open_hashtable<Key, void, NodeCount, Hasher, true>::for_each;
	};
};