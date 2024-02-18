#pragma once

#include "crstl/config.h"

#include "crstl/pair.h"

#include "crstl/hash.h"

#include "crstl/utility/memory_ops.h"

#include "crstl/utility/hashmap_common.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_INITIALIZER_LISTS)
#include <initializer_list>
#endif

// fixed_hashmap
//
// Fixed replacement for std::unordered_map
//
// fixed_hashmap doesn't allocate memory, instead manages an internal array
//
// - The number of nodes and buckets is specified at compile time
// - The bucket count should ideally be a power of 2 as it's faster to compute the bucket
// - The function signature for emplace varies with respect to std::unordered_map.
//   It behaves like std::vector's template or std::unordered_map's try_emplace
// - We have the option to store the key or just the hash, provided
//   1) The key is guaranteed to be unique
//   2) The user doesn't need the actual value of the key
// - There is no rehashing. Therefore iterators are stable with respect to insertion
// - 

crstl_module_export namespace crstl
{
	// Node in the hashmap. Stores metadata that is useful to know the current state of the node

	template<typename Key, typename T, typename LengthType>
	struct bucket_node
	{
		typedef LengthType length_type;

		enum node_meta : length_type
		{
			end   = (length_type)-1, // Indicates the last node in the list for any given bucket
			empty = (length_type)-2, // Indicates an empty node
		};

		size_t get_next() const
		{
			return next;
		}

		void set_next(size_t offset)
		{
			next = offset;
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

		pair<Key, T> key_value;

		length_type next;
	};

	template<typename Key, typename T, size_t BucketCount, typename LengthType>
	struct bucket_iterator
	{
	public:

		typedef bucket_iterator<Key, T, BucketCount, LengthType> this_type;
		typedef bucket_node<Key, T, LengthType> node_type;
		typedef LengthType length_type;

		static const length_type kInvalidNodeIndex = (length_type)-1;

		// We rely on this to avoid a branch when deleting a value
		static_assert(kInvalidNodeIndex == node_type::node_meta::end, "Make sure the invalid node and the value of end are the same");

		bucket_iterator(const length_type* buckets, const node_type* nodes, length_type bucket_start, const node_type* node_start)
			: m_buckets(buckets), m_bucket_index(bucket_start), m_data(nodes), m_node(node_start)
		{
			// A bucket index of BucketCount means invalid bucket, i.e. the end iterator
			crstl_assert(m_bucket_index <= BucketCount);
		}

		const pair<Key, T>* operator -> () const { crstl_assert(m_node != nullptr); return &(m_node->key_value); }
		
		const pair<Key, T>& operator * () const { crstl_assert(m_node != nullptr); return m_node->key_value; }
		
		bucket_iterator& operator ++ () { increment(); return *this; }
		
		bucket_iterator operator ++ (int) { bucket_iterator temp(*this); increment(); return temp; }
		
		bool operator == (const this_type& other) const { return m_bucket_index == other.m_bucket_index && m_node == other.m_node; }
		
		bool operator != (const this_type& other) const { return m_bucket_index != other.m_bucket_index || m_node != other.m_node; }
		
		const node_type* get_node() const { return m_node; }

		void increment()
		{
			if (m_node->is_valid())
			{
				m_node = &m_data[m_node->get_next()];
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
				m_node = &m_data[m_buckets[m_bucket_index]];
			}
			else
			{
				m_node = nullptr;
			}
		}

	private:

		const length_type* m_buckets;
		length_type m_bucket_index;

		const node_type* m_data; // Points to the main data. This is so we can index via the offset
		const node_type* m_node;
	};

	// Behavior to run when node exists already
	enum class exists_behavior : size_t
	{
		find,
		assign
	};

	// Whether to insert or emplace
	enum class insert_emplace : size_t
	{
		insert,
		emplace
	};

	// Use this selector class to determine whether to insert an already constructed object, or construct it in place given
		// the variadic arguments. This is so that emplace only constructs the object if it really needs to
	template<typename T, insert_emplace InsertEmplace>
	struct node_create_function_selector;

	template<typename T>
	struct node_create_function_selector<T, insert_emplace::insert>
	{
		template<typename NodeType, typename KeyType, typename ValueType>
		crstl_forceinline static void create(NodeType* new_node, KeyType&& key, ValueType&& value)
		{
			new_node->key_value = pair<KeyType, ValueType>(crstl::forward<KeyType>(key), crstl::forward<ValueType>(value));
		}
	};

	template<typename T>
	struct node_create_function_selector<T, insert_emplace::emplace>
	{
		template<typename NodeType, typename KeyType, typename... Args>
		crstl_forceinline static void create(NodeType* new_node, KeyType&& key, Args&&... args)
		{
			new_node->key_value = pair<KeyType, T>(crstl::forward<KeyType>(key), T(crstl::forward<Args>(args)...));
		}
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
	class fixed_hashmap
	{
	public:

		static_assert(NodeCount >= 1 && BucketCount >= 1, "Must have at least one node and one bucket");
		static_assert(NodeCount > BucketCount, "Must have at least one node per bucket");

		// Making length_type smaller than size_t has an effect on performance, so if we ever
		// change this make sure we profile first and make sure we distinguish storage from runtime
		typedef size_t                                                  length_type;
		typedef Key                                                     key_type;
		typedef T                                                       mapped_type;
		typedef pair<Key, T>                                            value_type;
		typedef size_t                                                  size_type;
		typedef Hasher                                                  hasher;
		typedef bucket_iterator<Key, T, BucketCount, length_type>       iterator;
		typedef const bucket_iterator<Key, T, BucketCount, length_type> const_iterator;
		typedef bucket_node<Key, T, length_type>                        node_type;

		static const length_type kInvalidNodeIndex = (length_type)-1;
		static const size_t kNodesPerBucket = NodeCount / BucketCount;

		crstl_constexpr14 fixed_hashmap() crstl_noexcept : m_length(0)
		{
			// (length_type)-1 is the invalid value for buckets
			memory_set(m_buckets, 0xff, BucketCount * sizeof(m_buckets[0]));

			for (size_t i = 0; i < NodeCount; ++i)
			{
				m_data[i].set_empty();
			}
		}

		crstl_constexpr14 fixed_hashmap(const fixed_hashmap& other) crstl_noexcept : fixed_hashmap()
		{
			for (const value_type& iter : other)
			{
				insert(iter);
			}

			m_length = other.m_length;
		}

		crstl_constexpr14 fixed_hashmap(std::initializer_list<value_type> ilist) crstl_noexcept : fixed_hashmap()
		{
			crstl_assert(ilist.size() <= NodeCount);

			for (const value_type& iter : ilist)
			{
				insert(crstl::move(iter));
			}
		}

		~fixed_hashmap() crstl_noexcept
		{
			// Only destroy the value, no need to destroy buckets or nodes
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				for (const value_type& iter : *this)
				{
					iter.second.~T();
				}
			}
		}

		crstl_constexpr14 fixed_hashmap& operator = (const fixed_hashmap& other)
		{
			clear();

			for (const value_type& iter : other)
			{
				insert(iter);
			}

			m_length = other.m_length;

			return *this;
		}

		crstl_constexpr14 iterator begin() crstl_noexcept
		{
			size_t first_valid_bucket = BucketCount; node_type* first_valid_node = nullptr;

			for (size_t i = 0; i < BucketCount; ++i)
			{
				if (m_buckets[i] != kInvalidNodeIndex)
				{
					first_valid_bucket = i;
					first_valid_node = &m_data[m_buckets[i]];
					break;
				}
			}

			return iterator(m_buckets, m_data, first_valid_bucket, first_valid_node);
		}
		
		crstl_constexpr14 const_iterator begin() const crstl_noexcept
		{
			size_t first_valid_bucket = BucketCount; const node_type* first_valid_node = nullptr;

			for (size_t i = 0; i < BucketCount; ++i)
			{
				if (m_buckets[i] != kInvalidNodeIndex)
				{
					first_valid_bucket = i;
					first_valid_node = &m_data[m_buckets[i]];
					break;
				}
			}

			return const_iterator(m_buckets, m_data, first_valid_bucket, first_valid_node);
		}

		crstl_constexpr14 const_iterator cbegin() const crstl_noexcept { return begin(); }
		
		crstl_constexpr14 void clear()
		{
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				for (const value_type& iter : *this)
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
			return emplace_impl<exists_behavior::find>(key, crstl::forward<Args>(args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace(Key&& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::find>(crstl::move(key), crstl::forward<Args>(args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace_or_assign(const Key& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::assign>(key, crstl::forward<Args>(args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace_or_assign(Key&& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::assign>(crstl::move(key), crstl::forward<Args>(args)...);
		}

		crstl_constexpr bool empty() const { return m_length == 0; }
		
		crstl_constexpr14 iterator end() { return iterator(m_buckets, m_data, BucketCount, nullptr); }

		crstl_constexpr const_iterator end() const { return const_iterator(m_buckets, m_data, BucketCount, nullptr); }

		crstl_constexpr const_iterator cend() const { return end(); }
		
		template<typename KeyType>
		crstl_noinline crstl_constexpr14 size_t erase(KeyType&& key)
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);
			
			size_t current_node_offset = m_buckets[bucket_index];
			size_t previous_node_offset = kInvalidNodeIndex;
			
			if (current_node_offset != kInvalidNodeIndex)
			{
				while (1)
				{
					node_type* current_node = &m_data[current_node_offset];

					crstl_assert(!current_node->is_empty());

					// If the key matches, delete this node and reconnect the rest
					if (current_node->key_value.first == key)
					{
						// Destroy existing value only if there's a destructor
						crstl_constexpr_if(!crstl_is_trivially_destructible(value_type))
						{
							current_node->key_value.~value_type();
						}

						// If there is no previous node, see what to do with the bucket
						if (previous_node_offset == kInvalidNodeIndex)
						{
							// We rely on the fact that the value of an end node is the same as the value
							// for an invalid bucket. This is equivalent to
							// m_buckets[bucket_index] = current_node->is_end() ? kInvalidNodeIndex : current_node->get_next();
							m_buckets[bucket_index] = current_node->get_next();
						}
						else
						{
							m_data[previous_node_offset].set_next(current_node->get_next());
						}

						current_node->set_empty();
						m_length--;

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
						previous_node_offset = current_node_offset;
						current_node_offset = current_node->get_next();
					}
				}
			}

			return 0;
		}

		template<typename KeyType>
		iterator find(KeyType&& key)
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);

			size_t current_node_offset = m_buckets[bucket_index];
		
			if (current_node_offset != kInvalidNodeIndex)
			{
				node_type* current_node = &m_data[current_node_offset];

				while (true)
				{
					if (current_node->key_value.first == key)
					{
						return iterator(m_buckets, m_data, bucket_index, current_node);
					}
					else if (current_node->is_end())
					{
						break;
					}
					else
					{
						current_node = &m_data[current_node->get_next()];
					}
				}
			}
		
			return end();
		}

		//-------
		// insert
		//-------

		pair<iterator, bool> insert(const pair<Key, T>& key_value) { return insert_impl<exists_behavior::find>(key_value.first, key_value.second); }

		pair<iterator, bool> insert(pair<Key, T>&& key_value) { return insert_impl<exists_behavior::find>(crstl::move(key_value.first), crstl::move(key_value.second)); }

		template<typename ValueType>
		pair<iterator, bool> insert(const Key& key, ValueType&& value) { return insert_impl<exists_behavior::find>(key, crstl::forward<ValueType>(value)); }

		template<typename ValueType>
		pair<iterator, bool> insert(Key&& key, ValueType&& value) { return insert_impl<exists_behavior::find>(crstl::move(key), crstl::forward<ValueType>(value)); }

		//-----------------
		// insert_or_assign
		//-----------------

		pair<iterator, bool> insert_or_assign(const pair<Key, T>& key_value) { return insert_impl<exists_behavior::assign>(key_value.first, key_value.second); }
		
		pair<iterator, bool> insert_or_assign(pair<Key, T>&& key_value) { return insert_impl<exists_behavior::assign>(crstl::move(key_value.first), crstl::move(key_value.second)); }
		
		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(const Key& key, ValueType&& value) { return insert_impl<exists_behavior::assign>(key, crstl::forward<ValueType>(value)); }

		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(Key&& key, ValueType&& value) { return insert_impl<exists_behavior::assign>(crstl::move(key), crstl::forward<ValueType>(value)); }

		size_t max_size() const { return sizeof(length_type) - 2; }

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

		// This function tries to find an empty space in the hashmap by checking whether the bucket is empty or finding an empty space in the
		// bucket if it already has something in it. If we find the object we're looking for, there are a series of different actions we can
		// take. If we are simply inserting (or emplacing) we just return the object that was there already
		template<exists_behavior Behavior, insert_emplace InsertEmplace, typename KeyType, typename... InsertEmplaceArgs>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> find_create_impl(KeyType&& key, InsertEmplaceArgs&&... insert_emplace_args)
		{
			// Get the type of the key_value member of the node. We need it to feed it to the macro
			typedef decltype(node_type::key_value) key_value_type;

			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket<BucketCount>(hash_value);

			size_t current_node_offset = m_buckets[bucket_index];

			// If the bucket is empty, there are no valid nodes. Nodes pointed at from a bucket are
			// necessarily either valid or the end of the list
			if (current_node_offset != kInvalidNodeIndex)
			{
				node_type* current_node = &m_data[current_node_offset];

				while (1)
				{
					// TODO For complex types keep the key and check if it is the same. If it is, then compare the value
					if (current_node->key_value.first == key)
					{
						// If our insert behavior is to assign, replace the existing value with the current one
						crstl_constexpr_if(Behavior == exists_behavior::assign)
						{
							// Destroy existing value
							crstl_constexpr_if(!crstl_is_trivially_destructible(key_value_type))
							{
								current_node->key_value.~key_value_type();
							}

							// Create the new one
							node_create_function_selector<T, InsertEmplace>::create(current_node, crstl::forward<KeyType>(key), crstl::forward<InsertEmplaceArgs>(insert_emplace_args)...);
						}

						// Return true in the second parameter if insertion happened, false otherwise
						return pair<iterator, bool>(iterator(m_buckets, m_data, bucket_index, current_node), Behavior == exists_behavior::assign);
					}
					else
					{
						// If we are the final node and we aren't the key, this is the end of the search. We must
						// now find an empty space to put it in
						if (current_node->is_end())
						{
							length_type empty_node_index = find_empty_node_index(bucket_index * kNodesPerBucket);
							node_type* empty_node = &m_data[empty_node_index];
							node_create_function_selector<T, InsertEmplace>::create(empty_node, crstl::forward<KeyType>(key), crstl::forward<InsertEmplaceArgs>(insert_emplace_args)...);
							empty_node->set_end();

							current_node->set_next(empty_node_index);

							m_length++;

							return pair<iterator, bool>(iterator(m_buckets, m_data, bucket_index, current_node), true);
						}
						// Otherwise, we might still find it in the next iteration
						else
						{
							current_node = &m_data[current_node->get_next()];
						}
					}
				}
			}
			else
			{
				// Find empty node index
				length_type empty_node_index = find_empty_node_index(bucket_index * kNodesPerBucket);

				// Make bucket point to it
				m_buckets[bucket_index] = empty_node_index;

				// Create new node and mark as end
				node_type* empty_node = &m_data[empty_node_index];
				node_create_function_selector<T, InsertEmplace>::create(empty_node, crstl::forward<KeyType>(key), crstl::forward<InsertEmplaceArgs>(insert_emplace_args)...);
				empty_node->set_end();

				m_length++;

				return pair<iterator, bool>(iterator(m_buckets, m_data, bucket_index, empty_node), true);
			}
		}

		template<exists_behavior Behavior, typename KeyType, typename... Args>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> emplace_impl(KeyType&& key, Args&&... args)
		{
			return find_create_impl<Behavior, insert_emplace::emplace>(crstl::forward<KeyType>(key), crstl::forward<Args>(args)...);
		}

		template<exists_behavior Behavior, typename KeyType, typename ValueType>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> insert_impl(KeyType&& key, ValueType&& value)
		{
			return find_create_impl<Behavior, insert_emplace::insert>(crstl::forward<KeyType>(key), crstl::forward<ValueType>(value));
		}

		// TODO Make this a bit more clever to find nodes faster
		crstl_forceinline length_type find_empty_node_index(size_t search_start)
		{
			length_type candidate_node_index = kInvalidNodeIndex;

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

		crstl_constexpr14 size_t compute_hash_value(const Key& key)
		{
			return hasher()(key);
		}

		length_type m_buckets[BucketCount];

		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { node_type m_data[NodeCount]; };
		};
		crstl_warning_anonymous_struct_union_end

		length_type m_length;
	};
};