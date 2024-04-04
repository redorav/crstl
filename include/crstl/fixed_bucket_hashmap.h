#pragma once

#include "crstl/config.h"

#include "crstl/pair.h"

#include "crstl/hash.h"

#include "crstl/utility/memory_ops.h"

#include "crstl/utility/hashmap_common.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

// fixed_bucket_hashmap
//
// Fixed replacement for std::unordered_map
//
// fixed_bucket_hashmap doesn't allocate memory, instead manages an internal array
//
// - The number of nodes and buckets is specified at compile time
// - The bucket count should ideally be a power of 2 as it's faster to compute the bucket
// - The function signature for emplace varies with respect to std::unordered_map.
//   It behaves like std::vector's emplace or std::unordered_map's try_emplace, meaning it won't construct the object if it's already in the hashmap
// - We have the option to store the key or just the hash, provided
//   1) The key is guaranteed to be unique
//   2) The user doesn't need the actual value of the key
// - There is no rehashing. Therefore iterators are stable with respect to insertion
// - 

crstl_module_export namespace crstl
{
	// Node in the hashmap. Stores metadata that is useful to know the current state of the node

	template<typename KeyValueType>
	struct bucket_node
	{
		enum node_meta : size_t
		{
			end   = (size_t)-1, // Indicates the last node in the list for any given bucket
			empty = (size_t)-2, // Indicates an empty node
		};

		size_t get_next() const
		{
			return next;
		}

		void set_next(size_t offset)
		{
			next = (node_meta)offset;
		}

		bool is_empty() const
		{
			return next == node_meta::empty;
		}

		bool is_end() const
		{
			return next == node_meta::end;
		}

		bool is_valid() const
		{
			return next < node_meta::end;
		}

		void set_empty()
		{
			next = node_meta::empty;
		}

		void set_end()
		{
			next = node_meta::end;
		}

		KeyValueType key_value;

		// Make this node_meta for debugging
		node_meta next;
	};

	template<typename KeyValueType, size_t BucketCount, bool IsConst>
	struct bucket_iterator
	{
	public:

		typedef bucket_iterator<KeyValueType, BucketCount, IsConst> this_type;
		typedef bucket_node<KeyValueType> node_type;
		typedef typename hashmap_type_select<IsConst, const KeyValueType*, KeyValueType*>::type pointer;
		typedef typename hashmap_type_select<IsConst, const KeyValueType&, KeyValueType&>::type reference;

		static const size_t kInvalidNodeIndex = (size_t)-1;

		// We rely on this to avoid a branch when deleting a value
		static_assert(kInvalidNodeIndex == node_type::node_meta::end, "Make sure the invalid node and the value of end are the same");

		bucket_iterator(const size_t* buckets, const node_type* nodes, size_t bucket_start, node_type* node_start, node_type* node_previous)
			: m_buckets(buckets), m_bucket_index(bucket_start), m_data(nodes), m_node(node_start), m_previous_node(node_previous)
		{
			// A bucket index of BucketCount means invalid bucket, i.e. the end iterator
			crstl_assert(m_bucket_index <= BucketCount);
		}

		pointer operator -> () const { crstl_assert(m_node != nullptr); return &(m_node->key_value); }
		
		reference operator * () const { crstl_assert(m_node != nullptr); return m_node->key_value; }
		
		bucket_iterator& operator ++ () { increment(); return *this; }
		
		bucket_iterator operator ++ (int) { bucket_iterator temp(*this); increment(); return temp; }
		
		// Check for node equality only, by definition if the nodes are equal the buckets have to be equal, and if the nodes
		// are not equal, then we don't care what the bucket index is, to be able to tell them apart
		bool operator == (const this_type& other) const { return m_node == other.m_node; }
		
		bool operator != (const this_type& other) const { return m_node != other.m_node; }
		
		node_type* get_node() const { return m_node; }

		node_type* get_previous_node() const { return m_previous_node; }

		size_t get_bucket_index() const { return m_bucket_index; }

		void increment()
		{
			if (m_node->is_valid())
			{
				m_node = (node_type*)&m_data[m_node->get_next()];
			}
			else
			{
				increment_bucket();
			}
		}
		
		void increment_bucket()
		{
			while (m_bucket_index < BucketCount)
			{
				m_bucket_index++;

				if (m_buckets[m_bucket_index] != kInvalidNodeIndex)
				{
					break;
				}
			}

			if (m_bucket_index < BucketCount)
			{
				m_node = (node_type*)&m_data[m_buckets[m_bucket_index]];
			}
			else
			{
				m_node = nullptr;
			}
		}

	private:

		const size_t* m_buckets;
		size_t m_bucket_index;

		const node_type* m_data; // Points to the main data. This is so we can index via the offset
		node_type* m_node;
		node_type* m_previous_node; // Used for when we call erase using an iterator, so we can relink the nodes
	};

	// It is recommended to use a power of 2 for buckets as it is faster to find
	template
	<
		typename Key,
		typename T,
		size_t NodeCount,
		size_t BucketCount = ((NodeCount / 2) > 0 ? (NodeCount / 2) : 1),
		typename Hasher = crstl::hash<Key>
	>
	class fixed_bucket_hashmap
	{
	public:

		static_assert(NodeCount >= 1 && BucketCount >= 1, "Must have at least one node and one bucket");

		typedef Key                                                 key_type;
		typedef T                                                   mapped_type;
		typedef pair<const Key, T>                                  key_value_type;
		typedef size_t                                              size_type;
		typedef Hasher                                              hasher;
		typedef bucket_iterator<key_value_type, BucketCount, false> iterator;
		typedef bucket_iterator<key_value_type, BucketCount, true>  const_iterator;
		typedef bucket_node<key_value_type>                         node_type;

		static const size_t kInvalidNodeIndex = (size_t)-1;
		static const size_t kNodesPerBucket = NodeCount / BucketCount;

		crstl_constexpr14 fixed_bucket_hashmap() crstl_noexcept : m_length(0)
		{
			// (size_t)-1 is the invalid value for buckets
			memory_set(m_buckets, 0xff, BucketCount * sizeof(m_buckets[0]));

			for (size_t i = 0; i < NodeCount; ++i)
			{
				m_data[i].set_empty();
			}
		}

		crstl_constexpr14 fixed_bucket_hashmap(const fixed_bucket_hashmap& other) crstl_noexcept : fixed_bucket_hashmap()
		{
			for (const key_value_type& iter : other)
			{
				insert(iter);
			}
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		crstl_constexpr14 fixed_bucket_hashmap(std::initializer_list<key_value_type> ilist) crstl_noexcept : fixed_bucket_hashmap()
		{
			crstl_assert(ilist.size() <= NodeCount);

			for (const key_value_type& iter : ilist)
			{
				insert(iter);
			}
		}

#endif

		~fixed_bucket_hashmap() crstl_noexcept
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

		crstl_constexpr14 fixed_bucket_hashmap& operator = (const fixed_bucket_hashmap& other)
		{
			clear();

			for (const key_value_type& iter : other)
			{
				insert(iter);
			}

			return *this;
		}

		crstl_constexpr14 iterator begin() crstl_noexcept
		{
			size_t first_valid_bucket = BucketCount;
			node_type* first_valid_node = nullptr;

			for (size_t i = 0; i < BucketCount; ++i)
			{
				if (m_buckets[i] != kInvalidNodeIndex)
				{
					first_valid_bucket = i;
					first_valid_node = &m_data[m_buckets[i]];
					break;
				}
			}

			return iterator(m_buckets, m_data, first_valid_bucket, first_valid_node, nullptr);
		}
		
		crstl_constexpr14 const_iterator begin() const crstl_noexcept
		{
			size_t first_valid_bucket = BucketCount;
			node_type* first_valid_node = nullptr;

			for (size_t i = 0; i < BucketCount; ++i)
			{
				if (m_buckets[i] != kInvalidNodeIndex)
				{
					first_valid_bucket = i;
					first_valid_node = (node_type*)&m_data[m_buckets[i]];
					break;
				}
			}

			return const_iterator(m_buckets, m_data, first_valid_bucket, first_valid_node, nullptr);
		}

		crstl_constexpr14 const_iterator cbegin() const crstl_noexcept { return begin(); }
		
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

			memory_set(m_buckets, 0xff, BucketCount * sizeof(m_buckets[0]));
		
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
		
		crstl_constexpr14 iterator end() { return iterator(m_buckets, m_data, BucketCount, nullptr, nullptr); }

		crstl_constexpr const_iterator end() const { return const_iterator(m_buckets, m_data, BucketCount, nullptr, nullptr); }

		crstl_constexpr const_iterator cend() const { return end(); }
		
		crstl_constexpr14 iterator erase(iterator pos)
		{
			crstl_assert(pos != end());
			iterator next_iter = pos; ++next_iter;
			erase_iter_impl(pos.get_node(), pos.get_previous_node(), pos.get_bucket_index());
			return next_iter;
		}

		crstl_constexpr14 const_iterator erase(const_iterator pos)
		{
			crstl_assert(pos != cend());
			const_iterator next_iter = pos; ++next_iter;
			erase_iter_impl(pos.get_node(), pos.get_previous_node(), pos.get_bucket_index());
			return next_iter;
		}

		template<typename KeyType>
		crstl_forceinline crstl_constexpr14 size_t erase(KeyType&& key)
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);
			
			size_t current_node_offset = m_buckets[bucket_index];
			
			if (current_node_offset != kInvalidNodeIndex)
			{
				while (true)
				{
					node_type* current_node = &m_data[current_node_offset];
					node_type* previous_node = nullptr;

					crstl_assert(!current_node->is_empty());

					// If the key matches, delete this node and reconnect the rest
					if (current_node->key_value.first == key)
					{
						erase_iter_impl(current_node, previous_node, bucket_index);
						return 1;
					}
					// If this is the last node, we've finished our search
					else if (current_node->is_end())
					{
						break;
					}
					// Otherwise, look for the next node
					else
					{
						previous_node = current_node;
						current_node_offset = current_node->get_next();
					}
				}
			}

			return 0;
		}

		template<typename KeyType>
		iterator find(const KeyType& key)
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);
			node_type* found_node; node_type* previous_node;
			find_impl(bucket_index, key, found_node, previous_node);
			return iterator(m_buckets, m_data, bucket_index, found_node, previous_node);
		}

		template<typename KeyType>
		const_iterator find(const KeyType& key) const
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);
			node_type* found_node; node_type* previous_node;
			find_impl(bucket_index, key, found_node, previous_node);
			return const_iterator(m_buckets, m_data, bucket_index, found_node, previous_node);
		}

		//-------
		// insert
		//-------

		pair<iterator, bool> insert(const key_value_type& key_value)
		{
			return insert_impl<exists_behavior::find>(key_value.first, key_value.second);
		}

		pair<iterator, bool> insert(key_value_type&& key_value)
		{
			return insert_impl<exists_behavior::find>(crstl_forward(key_value_type, key_value).first, crstl_forward(key_value_type, key_value).second);
		}

		template<typename ValueType>
		pair<iterator, bool> insert(const Key& key, ValueType&& value)
		{
			return insert_impl<exists_behavior::find>(key, crstl_forward(ValueType, value));
		}

		template<typename ValueType>
		pair<iterator, bool> insert(Key&& key, ValueType&& value)
		{
			return insert_impl<exists_behavior::find>(crstl_forward(const Key, key), crstl_forward(ValueType, value));
		}

		//-----------------
		// insert_or_assign
		//-----------------

		pair<iterator, bool> insert_or_assign(const key_value_type& key_value)
		{
			return insert_impl<exists_behavior::assign>(key_value.first, key_value.second);
		}

		pair<iterator, bool> insert_or_assign(key_value_type&& key_value)
		{
			return insert_impl<exists_behavior::assign>(crstl_forward(key_value_type, key_value).first, crstl_forward(key_value_type, key_value).second);
		}
		
		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(const Key& key, ValueType&& value)
		{
			return insert_impl<exists_behavior::assign>(key, crstl_forward(ValueType, value));
		}

		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(Key&& key, ValueType&& value)
		{
			return insert_impl<exists_behavior::assign>(crstl_forward(const Key, key), crstl_forward(ValueType, value));
		}

		size_t max_size() const { return ((size_t)-1) - 2; }

		crstl_constexpr size_t size() const
		{
			return m_length;
		}

		crstl_constexpr14 void debug_validate_empty() const
		{
			for (size_t i = 0; i < BucketCount; ++i)
			{
				crstl_assert(m_buckets[i] == kInvalidNodeIndex);
			}

			for (size_t i = 0; i < NodeCount; ++i)
			{
				crstl_assert(m_data[i].is_empty());
			}
		
			crstl_assert(m_length == 0);
		}

		float load_factor() const
		{
			return (float)m_length / (float)NodeCount;
		}

	private:

		// Implementation of deletion given a node, its previous node (if any) and the bucket index
		crstl_forceinline crstl_constexpr14 void erase_iter_impl(node_type* current_node, node_type* previous_node, size_t bucket_index)
		{
			// Destroy existing value only if there's a destructor
			crstl_constexpr_if(!crstl_is_trivially_destructible(key_value_type))
			{
				current_node->key_value.~key_value_type();
			}

			// Relink previous node if we have one
			if (previous_node)
			{
				previous_node->set_next(current_node->get_next());
			}
			else
			{
				// We rely on the fact that the value of an end node is the same as the value
				// for an invalid bucket. This is equivalent to
				// m_buckets[bucket_index] = current_node->is_end() ? kInvalidNodeIndex : current_node->get_next();
				m_buckets[bucket_index] = current_node->get_next();
			}

			current_node->set_empty();
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

			size_t current_node_offset = m_buckets[bucket_index];

			// If the bucket is empty, there are no valid nodes. Nodes pointed at from a bucket are
			// necessarily either valid or the end of the list
			if (current_node_offset != kInvalidNodeIndex)
			{
				node_type* current_node = &m_data[current_node_offset];
				node_type* previous_node = nullptr;

				while (true)
				{
					// TODO For complex types keep the key and check if it is the same. If it is, then compare the value
					if (current_node->key_value.first == key)
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
							node_create_selector<KeyValueType, T, InsertEmplace>::create(current_node, crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
						}

						// Return true in the second parameter if insertion happened, false otherwise
						return pair<iterator, bool>(iterator(m_buckets, m_data, bucket_index, current_node, previous_node), Behavior == exists_behavior::assign);
					}
					else
					{
						// If we are the final node and we aren't the key, this is the end of the search. We must
						// now find an empty space to put it in
						if (current_node->is_end())
						{
							size_t empty_node_index = find_empty_node_index(bucket_index * kNodesPerBucket);
							node_type* empty_node = &m_data[empty_node_index];
							node_create_selector<KeyValueType, T, InsertEmplace>::create(empty_node, crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
							empty_node->set_end();

							current_node->set_next(empty_node_index);

							m_length++;

							return pair<iterator, bool>(iterator(m_buckets, m_data, bucket_index, current_node, previous_node), true);
						}
						// Otherwise, we might still find it in the next iteration
						else
						{
							previous_node = current_node;
							current_node = &m_data[current_node->get_next()];
						}
					}
				}
			}
			else
			{
				// Find empty node index
				size_t empty_node_index = find_empty_node_index(bucket_index * kNodesPerBucket);

				// Make bucket point to it
				m_buckets[bucket_index] = empty_node_index;

				// Create new node and mark as end
				node_type* empty_node = &m_data[empty_node_index];
				node_create_selector<KeyValueType, T, InsertEmplace>::create(empty_node, crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
				empty_node->set_end();

				m_length++;

				return pair<iterator, bool>(iterator(m_buckets, m_data, bucket_index, empty_node, nullptr), true);
			}
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

		// TODO Make this a bit more clever to find nodes faster
		crstl_forceinline size_t find_empty_node_index(size_t search_start)
		{
			size_t candidate_node_index = kInvalidNodeIndex;

			for (size_t i = search_start; i < NodeCount; ++i)
			{
				if (m_data[i].is_empty())
				{
					candidate_node_index = i;
					goto found_candidate;
				}
			}

			for (size_t i = 0; i < search_start; ++i)
			{
				if (m_data[i].is_empty())
				{
					candidate_node_index = i;
					goto found_candidate;
				}
			}

		found_candidate:

			crstl_assert(candidate_node_index != kInvalidNodeIndex);

			return candidate_node_index;
		}

		template<typename KeyType>
		crstl_forceinline void find_impl(size_t bucket_index, const KeyType& key, node_type*& found_node, node_type*& previous_node) const
		{
			found_node = nullptr;
			previous_node = nullptr;

			size_t current_node_offset = m_buckets[bucket_index];

			if (current_node_offset != kInvalidNodeIndex)
			{
				node_type* current_node = (node_type*)&m_data[current_node_offset];

				while (true)
				{
					if (current_node->key_value.first == key)
					{
						found_node = current_node;
						return;
					}
					else if (current_node->is_end())
					{
						return;
					}
					else
					{
						previous_node = current_node;
						current_node = (node_type*)&m_data[current_node->get_next()];
					}
				}
			}
		}

		static crstl_constexpr14 size_t compute_hash_value(const Key& key)
		{
			return hasher()(key);
		}

		size_t m_buckets[BucketCount];

		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { node_type m_data[NodeCount]; };
		};
		crstl_warning_anonymous_struct_union_end

		size_t m_length;
	};
};