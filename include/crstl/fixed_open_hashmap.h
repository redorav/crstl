#pragma once

#include "crstl/config.h"

#include "crstl/pair.h"

#include "crstl/hash.h"

#include "crstl/utility/memory_ops.h"

#include "crstl/utility/hashmap_common.h"

#include "crstl/type_array.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

crstl_module_export namespace crstl
{
	template<typename KeyValueType>
	struct open_node
	{
		enum node_meta : size_t
		{
			empty      = 0, // Indicates an empty node
			tombstone  = 1, // Indicates node was deleted
			first_hash = 2, // First valid hash
		};
		
		size_t get_hash() const { return (size_t)hash; }

		bool is_empty() const { return hash == node_meta::empty; }

		bool is_tombstone() const { return hash == node_meta::tombstone; }

		bool is_valid() const { return hash >= node_meta::first_hash; }

		void set_hash(size_t hash_value) { hash = hash_value; }

		void set_empty() { hash = node_meta::empty; }

		void set_tombstone() { hash = node_meta::tombstone; }

		KeyValueType key_value;

		size_t hash;
	};

	template<typename KeyValueType, size_t NodeCount, bool IsConst>
	struct open_iterator
	{
	public:

		typedef open_iterator<KeyValueType, NodeCount, IsConst> this_type;
		typedef open_node<KeyValueType> node_type;
		typedef typename hashmap_type_select<IsConst, const KeyValueType*, KeyValueType*>::type pointer;
		typedef typename hashmap_type_select<IsConst, const KeyValueType&, KeyValueType&>::type reference;

		open_iterator(const node_type* data, node_type* node) : m_data(data), m_node(node) {}

		pointer operator -> () const { crstl_assert(m_node != nullptr); return &(m_node->key_value); }

		reference operator * () const { crstl_assert(m_node != nullptr); return m_node->key_value; }

		open_iterator& operator ++ () { increment(); return *this; }
		
		open_iterator operator ++ (int) { open_iterator temp(*this); increment(); return temp; }

		bool operator == (const this_type& other) const { return m_node == other.m_node; }

		bool operator != (const this_type& other) const { return m_node != other.m_node; }

		node_type* get_node() const { return m_node; }

		void increment()
		{
			do
			{
				m_node++;
			} while (!m_node->is_valid() && m_node < m_data + NodeCount);
		}

	private:

		const node_type* m_data;

		node_type* m_node;
	};

	// It is recommended to use a power of 2 for buckets as it is faster to find
	template
	<
		typename Key,
		typename T,
		size_t NodeCount,
		size_t BucketCount = NodeCount,//((NodeCount / 2) > 0 ? (NodeCount / 2) : 1),
		typename Hasher = crstl::hash<Key>
	>
	class fixed_open_hashmap
	{
	public:

		static_assert(NodeCount >= 1 && BucketCount >= 1, "Must have at least one node and one bucket");

		typedef Key                                             key_type;
		typedef T                                               mapped_type;
		typedef pair<const Key, T>                              key_value_type;
		typedef size_t                                          size_type;
		typedef Hasher                                          hasher;
		typedef open_iterator<key_value_type, NodeCount, false> iterator;
		typedef open_iterator<key_value_type, NodeCount, true>  const_iterator;
		typedef open_node<key_value_type>                       node_type;

		static const size_t kNodesPerBucket = NodeCount / BucketCount;

		crstl_constexpr14 fixed_open_hashmap() crstl_noexcept : m_length(0)
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

		crstl_nodiscard
		crstl_constexpr14 iterator begin() crstl_noexcept
		{
			return iterator(m_data, begin_impl());
		}

		crstl_nodiscard
		crstl_constexpr14 const_iterator begin() const crstl_noexcept
		{
			return const_iterator(m_data, begin_impl());
		}

		crstl_nodiscard
		crstl_constexpr14 const_iterator cbegin() const crstl_noexcept
		{
			return const_iterator(m_data, begin_impl());
		}

		node_type* begin_impl() const
		{
			if (empty())
			{
				return (node_type*)(m_data + NodeCount);
			}
			else
			{
				node_type* valid_node = (node_type*)m_data;
				while (true)
				{
					if (valid_node->is_valid())
					{
						return valid_node;
					}

					valid_node++;
				}
			}
		}

		crstl_constexpr14 void clear()
		{
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				for (const key_value_type& iter : *this)
				{
					iter.second.~T();
				}
			}

			// Clear m_data
			for (size_t i = 0; i < NodeCount; ++i)
			{
				m_data[i].set_empty();
			}

			m_length = 0;
		}

		//--------
		// emplace
		//--------

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace(const Key& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::find>(key, crstl_forward(Args, args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace(Key&& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::find>(crstl_forward(const Key, key), crstl_forward(Args, args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace_or_assign(const Key& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::assign>(key, crstl_forward(Args, args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace_or_assign(Key&& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::assign>(crstl_forward(const Key, key), crstl_forward(Args, args)...);
		}

		crstl_constexpr bool empty() const { return m_length == 0; }

		crstl_constexpr14 iterator end() { return iterator(m_data, (node_type*)(m_data + NodeCount)); }

		crstl_constexpr const_iterator end() const { return const_iterator(m_data, (node_type*)(m_data + NodeCount)); }

		crstl_constexpr const_iterator cend() const { return const_iterator(m_data, (node_type*)(m_data + NodeCount)); }

		crstl_constexpr14 iterator erase(iterator pos)
		{
			crstl_assert(pos != end());
			iterator next_iter = pos; ++next_iter;
			erase_iter_impl(pos.get_node());
			return next_iter;
		}
		
		crstl_constexpr14 const_iterator erase(const_iterator pos)
		{
			crstl_assert(pos != cend());
			const_iterator next_iter = pos; ++next_iter;
			erase_iter_impl(pos.get_node());
			return next_iter;
		}

		template<typename KeyType>
		crstl_constexpr14 size_t erase(KeyType&& key)
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);

			node_type* start_node = m_data + bucket_index * kNodesPerBucket;
			node_type* crstl_restrict current_node = start_node;
			const node_type* end_node = m_data + NodeCount;

			do
			{
				// If this is the last node, we've finished our search
				if (current_node->is_empty())
				{
					break;
				}
				// If the key matches, delete this node and reconnect the rest
				else if (current_node->key_value.first == key)
				{
					erase_iter_impl(current_node);
					return 1;
				}

				// Otherwise, look for the next node
				current_node++;
				current_node = (current_node == end_node) ? m_data : current_node;
			} while (current_node != start_node);

			return 0;
		}

		template<typename KeyType>
		crstl_nodiscard iterator find(const KeyType& key) crstl_noexcept
		{
			node_type* found_node = find_impl(key);
			return iterator(m_data, found_node);
		}

		template<typename KeyType>
		crstl_nodiscard const_iterator find(const KeyType& key) const crstl_noexcept
		{
			node_type* found_node = find_impl(key);
			return const_iterator(m_data, found_node);
		}

		//-------
		// insert
		//-------

		pair<iterator, bool> insert(const key_value_type& key_value) { return insert_impl<exists_behavior::find>(key_value.first, key_value.second); }

		pair<iterator, bool> insert(key_value_type&& key_value) { return insert_impl<exists_behavior::find>(crstl_forward(const Key, key_value.first), crstl_forward(T, key_value.second)); }

		template<typename ValueType>
		pair<iterator, bool> insert(const Key& key, ValueType&& value) { return insert_impl<exists_behavior::find>(key, crstl_forward(ValueType, value)); }

		template<typename ValueType>
		pair<iterator, bool> insert(Key&& key, ValueType&& value) { return insert_impl<exists_behavior::find>(crstl_forward(const Key, key), crstl_forward(ValueType, value)); }

		//-----------------
		// insert_or_assign
		//-----------------

		pair<iterator, bool> insert_or_assign(const key_value_type& key_value) { return insert_impl<exists_behavior::assign>(key_value.first, key_value.second); }

		pair<iterator, bool> insert_or_assign(key_value_type&& key_value) { return insert_impl<exists_behavior::assign>(crstl_forward(const Key, key_value.first), crstl_move(key_value.second)); }

		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(const Key& key, ValueType&& value) { return insert_impl<exists_behavior::assign>(key, crstl_forward(ValueType, value)); }

		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(Key&& key, ValueType&& value) { return insert_impl<exists_behavior::assign>(crstl_forward(const Key, key), crstl_forward(ValueType, value)); }

		crstl_nodiscard
		size_t size() const { return m_length; }

	private:

		// Implementation of deletion given a node, its previous node (if any) and the bucket index
		crstl_forceinline crstl_constexpr14 void erase_iter_impl(node_type* current_node)
		{
			// Destroy existing value only if there's a destructor
			crstl_constexpr_if(!crstl_is_trivially_destructible(key_value_type))
			{
				current_node->key_value.~key_value_type();
			}

			current_node->set_tombstone();
			m_length--;
		}

		// This function tries to find an empty space in the hashmap by checking whether the bucket is empty or finding an empty space in the
		// bucket if it already has something in it. If we find the object we're looking for, there are a series of different actions we can
		// take. If we are simply inserting (or emplacing) we just return the object that was there already
		template<exists_behavior Behavior, insert_emplace InsertEmplace, typename KeyType, typename... InsertEmplaceArgs>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> find_create_impl(KeyType&& key, InsertEmplaceArgs&&... insert_emplace_args)
		{
			// Get the type of the key_value member of the node. We need it to feed it to the macro
			typedef decltype(node_type::key_value) KeyValueType;

			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);

			node_type* start_node = m_data + bucket_index * kNodesPerBucket;
			node_type* crstl_restrict current_node = start_node;
			node_type* first_tombstone = nullptr;
			const node_type* end_node = m_data + NodeCount;

			do
			{
				if (current_node->is_empty())
				{
					// Keep track of the first tombstone we encounter so that if we don't find the key we're looking for, we can insert it here
					node_type* empty_node = first_tombstone ? first_tombstone : (node_type*)current_node;

					empty_node->set_hash(hash_value);
					node_create_selector<KeyValueType, T, InsertEmplace>::create(empty_node, crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
					m_length++;
					return { iterator(m_data, empty_node), true };
				}
				else if (current_node->is_tombstone())
				{
					first_tombstone = first_tombstone ? first_tombstone : (node_type*)current_node;
				}
				else if (key == current_node->key_value.first)
				{
					// If our insert behavior is to assign, replace the existing value with the current one
					crstl_constexpr_if(Behavior == exists_behavior::assign)
					{
						// Destroy existing value
						crstl_constexpr_if(!crstl_is_trivially_destructible(KeyValueType))
						{
							current_node->key_value.~KeyValueType();
						}

						// Create the new one
						current_node->set_hash(hash_value);
						node_create_selector<KeyValueType, T, InsertEmplace>::create(current_node, crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
					}

					return { iterator(m_data, current_node), Behavior == exists_behavior::assign };
				}

				current_node++;
				current_node = (current_node == end_node) ? m_data : current_node;

			} while(current_node != start_node);

			return { iterator(m_data, nullptr), false };
		}

		template<exists_behavior Behavior, typename KeyType, typename... Args>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> emplace_impl(KeyType&& key, Args&&... args)
		{
			return find_create_impl<Behavior, insert_emplace::emplace>(crstl_forward(KeyType, key), crstl_forward(Args, args)...);
		}

		template<exists_behavior Behavior, typename KeyType, typename ValueType>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> insert_impl(KeyType&& key, ValueType&& value)
		{
			return find_create_impl<Behavior, insert_emplace::insert>(crstl_forward(KeyType, key), crstl_forward(ValueType, value));
		}

		template<typename KeyType>
		crstl_forceinline node_type* find_impl(const KeyType& key) const
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);

			node_type* const data = (node_type*)m_data;
			node_type* const start_node = data + bucket_index * kNodesPerBucket;
			node_type* const end_node = data + NodeCount;
			node_type* crstl_restrict current_node = start_node;

			do
			{
				if (current_node->is_empty())
				{
					return end_node;
				}
			
				if (key == current_node->key_value.first)
				{
					return current_node;
				}
			
				current_node++;
				current_node = (current_node == end_node) ? data : current_node;
			} while(current_node != start_node);

			return end_node;
		}

		static crstl_constexpr14 size_t compute_hash_value(const Key& key)
		{
			size_t hash_value = hasher()(key);

			// Two values will have more collisions than some others
			return hash_value > node_type::first_hash ? hash_value : hash_value + node_type::first_hash;
		}

		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { node_type m_data[NodeCount]; };
		};
		crstl_warning_anonymous_struct_union_end

		size_t m_length;
	};
};