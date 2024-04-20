#pragma once

#include "crstl/bit.h"

namespace crstl
{
	template<typename KeyValueType>
	struct open_node
	{
		enum node_meta
		{
			empty      = 0, // Indicates an empty node
			tombstone  = 1, // Indicates node was deleted
			valid      = 2, // First valid hash
		};
		
		bool is_empty() const { return meta == node_meta::empty; }

		bool is_tombstone() const { return meta == node_meta::tombstone; }

		bool is_valid() const { return meta > node_meta::tombstone; }

		void set_valid()
		{
			meta = node_meta::valid; 
		}

		void set_empty() { meta = (int)node_meta::empty; }

		void set_tombstone() { meta = (int)node_meta::tombstone; }

		unsigned char meta;

		KeyValueType key_value;
	};

	template<typename KeyValueType, bool IsConst>
	struct open_iterator
	{
	public:

		typedef open_iterator<KeyValueType, IsConst> this_type;
		typedef open_node<KeyValueType> node_type;
		typedef typename hashmap_type_select<IsConst, const KeyValueType*, KeyValueType*>::type pointer;
		typedef typename hashmap_type_select<IsConst, const KeyValueType&, KeyValueType&>::type reference;

		open_iterator(const node_type* data, const node_type* end, node_type* node) : m_data(data), m_end(end), m_node(node) {}

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
			}
			// The order of operations is important here, we cannot dereference the end node to check
			// if it's valid or not before we've checked if it's within the valid range of data
			while (m_node < m_end && !m_node->is_valid());
		}

	private:

		const node_type* m_data;

		const node_type* m_end;

		node_type* m_node;
	};

	template<typename HashmapStorage>
	class open_hashmap_base : public HashmapStorage
	{
	public:

		typedef HashmapStorage storage_type;
		typedef open_hashmap_base this_type;

		typedef typename HashmapStorage::key_type       key_type;
		typedef typename HashmapStorage::mapped_type    mapped_type;
		typedef typename HashmapStorage::key_value_type key_value_type;
		typedef typename HashmapStorage::hasher         hasher;
		typedef typename HashmapStorage::iterator       iterator;
		typedef typename HashmapStorage::const_iterator const_iterator;
		typedef typename HashmapStorage::node_type      node_type;

		using storage_type::m_data;
		using storage_type::m_length;

		using storage_type::compute_bucket;
		using storage_type::get_capacity;
		using storage_type::reallocate_rehash_if_length_above_threshold;

		crstl_nodiscard
		crstl_constexpr14 iterator begin() crstl_noexcept
		{
			return iterator(m_data, m_data + get_capacity(), begin_impl());
		}

		crstl_nodiscard
		crstl_constexpr14 const_iterator begin() const crstl_noexcept
		{
			return const_iterator(m_data, m_data + get_capacity(), begin_impl());
		}

		crstl_nodiscard
		crstl_constexpr14 const_iterator cbegin() const crstl_noexcept
		{
			return const_iterator(m_data, m_data + get_capacity(), begin_impl());
		}

		crstl_constexpr14 void clear()
		{
			crstl_constexpr_if(!crstl_is_trivially_destructible(mapped_type))
			{
				for (const key_value_type& iter : *this)
				{
					iter.second.~mapped_type();
				}
			}

			// Clear m_data. This helps if we're going to reuse the hashmap to clear any tombstones
			for (size_t i = 0; i < get_capacity(); ++i)
			{
				m_data[i].set_empty();
			}

			m_length = 0;
		}

		crstl_constexpr bool empty() const { return m_length == 0; }

		crstl_nodiscard
		crstl_constexpr14 iterator end() { return iterator(m_data, (node_type*)(m_data + get_capacity()), (node_type*)(m_data + get_capacity())); }

		crstl_nodiscard
		crstl_constexpr const_iterator end() const { return const_iterator(m_data, (node_type*)(m_data + get_capacity()), (node_type*)(m_data + get_capacity())); }

		crstl_nodiscard
		crstl_constexpr const_iterator cend() const { return const_iterator(m_data, (node_type*)(m_data + get_capacity()), (node_type*)(m_data + get_capacity())); }

#if defined(CRSTL_VARIADIC_TEMPLATES)

		//--------
		// emplace
		//--------

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace(const key_type& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::find>(key, crstl_forward(Args, args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace(key_type&& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::find>(crstl_forward(const key_type, key), crstl_forward(Args, args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace_or_assign(const key_type& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::assign>(key, crstl_forward(Args, args)...);
		}

		template<typename... Args>
		crstl_constexpr14 pair<iterator, bool> emplace_or_assign(key_type&& key, Args&&... args)
		{
			return emplace_impl<exists_behavior::assign>(crstl_forward(const key_type, key), crstl_forward(Args, args)...);
		}

#endif

		//------
		// erase
		//------

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
			const size_t bucket_index = compute_bucket(hash_value);

			node_type* start_node = m_data + bucket_index;
			node_type* current_node = start_node;
			const node_type* end_node = m_data + get_capacity();

			do
			{
				// If this is the last node, we've finished our search
				if (current_node->is_empty())
				{
					break;
				}
				else if (current_node->is_tombstone())
				{
					// Skip over tombstones
				}
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
			return iterator(m_data, m_data + get_capacity(), found_node);
		}

		template<typename KeyType>
		crstl_nodiscard const_iterator find(const KeyType& key) const crstl_noexcept
		{
			node_type* found_node = find_impl(key);
			return const_iterator(m_data, m_data + get_capacity(), found_node);
		}

		//-------
		// insert
		//-------

		pair<iterator, bool> insert(const key_value_type& key_value) { return insert_impl<exists_behavior::find>(key_value.first, key_value.second); }

		pair<iterator, bool> insert(key_value_type&& key_value) { return insert_impl<exists_behavior::find>(crstl_forward(const key_type, key_value.first), crstl_move(key_value.second)); }

		template<typename ValueType>
		pair<iterator, bool> insert(const key_type& key, ValueType&& value) { return insert_impl<exists_behavior::find>(key, crstl_forward(ValueType, value)); }

		template<typename ValueType>
		pair<iterator, bool> insert(key_type&& key, ValueType&& value) { return insert_impl<exists_behavior::find>(crstl_forward(const key_type, key), crstl_forward(ValueType, value)); }

		//-----------------
		// insert_or_assign
		//-----------------

		pair<iterator, bool> insert_or_assign(const key_value_type& key_value) { return insert_impl<exists_behavior::assign>(key_value.first, key_value.second); }

		pair<iterator, bool> insert_or_assign(key_value_type&& key_value) { return insert_impl<exists_behavior::assign>(crstl_forward(const key_type, key_value.first), crstl_move(key_value.second)); }

		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(const key_type& key, ValueType&& value) { return insert_impl<exists_behavior::assign>(key, crstl_forward(ValueType, value)); }

		template<typename ValueType>
		pair<iterator, bool> insert_or_assign(key_type&& key, ValueType&& value) { return insert_impl<exists_behavior::assign>(crstl_forward(const key_type, key), crstl_forward(ValueType, value)); }

		bool reserve(size_t capacity)
		{
			//reserve_capacity();
		}

		crstl_nodiscard
		size_t size() const { return m_length; }

	protected:

		template<exists_behavior::t Behavior, typename KeyType, typename... Args>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> emplace_impl(KeyType&& key, Args&&... args)
		{
			return find_create_reallocate<Behavior, insert_emplace::emplace>(crstl_forward(KeyType, key), crstl_forward(Args, args)...);
		}

		template<exists_behavior::t Behavior, typename KeyType, typename ValueType>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> insert_impl(KeyType&& key, ValueType&& value)
		{
			return find_create_reallocate<Behavior, insert_emplace::insert>(crstl_forward(KeyType, key), crstl_forward(ValueType, value));
		}

		template<exists_behavior::t Behavior, insert_emplace::t InsertEmplace, typename KeyType, typename... InsertEmplaceArgs>
		crstl_forceinline crstl_constexpr14 pair<iterator, bool> find_create_reallocate(KeyType&& key, InsertEmplaceArgs&&... insert_emplace_args)
		{
			reallocate_rehash_if_length_above_threshold([this](node_type* crstl_restrict current_node, const node_type* const end_node)
			{
				for (; current_node != end_node; ++current_node)
				{
					if (current_node->is_valid())
					{
						reinsert_impl(crstl_move(current_node->key_value.first), crstl_move(current_node->key_value.second));

						crstl_constexpr_if(!crstl_is_trivially_destructible(node_type))
						{
							current_node->~node_type();
						}
					}
				}
			});

			return find_create_impl<Behavior, InsertEmplace>(crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
		}

		// Optimized version of insert when we know we are batch inserting nodes into a clean hashmap with no tombstones
		// We can skip iterator construction, the search for a tombstone and even the comparison with the key
		template<typename KeyType, typename ValueType>
		inline crstl_constexpr14 void reinsert_impl(KeyType&& key, ValueType&& value)
		{
			typedef decltype(node_type::key_value) KeyValueType;

			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket(hash_value);
			crstl_assert(bucket_index <= get_capacity());

			node_type* const start_node = m_data + bucket_index;
			node_type* const end_node = m_data + get_capacity();
			node_type* current_node = start_node;

			do
			{
				if (current_node->is_empty())
				{
					crstl_placement_new((void*)&(current_node->key_value)) KeyValueType(crstl_move(key), crstl_move(value));
					current_node->set_valid();
					m_length++;
					return;
				}

				current_node++;
				current_node = (current_node == end_node) ? m_data : current_node;
			} while(current_node != start_node);
		}

		// This function tries to find an empty space in the hashmap by checking whether the bucket is empty or finding an empty space in the
		// bucket if it already has something in it. If we find the object we're looking for, there are a series of different actions we can
		// take. If we are simply inserting (or emplacing) we just return the object that was there already
		template<exists_behavior::t Behavior, insert_emplace::t InsertEmplace, typename KeyType, typename... InsertEmplaceArgs>
		inline crstl_constexpr14 pair<iterator, bool> find_create_impl(KeyType&& key, InsertEmplaceArgs&&... insert_emplace_args)
		{
			// Get the type of the key_value member of the node. We need it to feed it to the macro
			typedef decltype(node_type::key_value) KeyValueType;

			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket(hash_value);
			crstl_assert(bucket_index <= get_capacity());
			
			node_type* const start_node = m_data + bucket_index;
			node_type* const end_node = m_data + get_capacity();
			node_type* current_node = start_node;
			node_type* first_tombstone = nullptr;

			do
			{
				if (current_node->is_empty())
				{
					// Keep track of the first tombstone we encounter so that if we don't find the key we're looking for, we can insert it here
					node_type* empty_node = first_tombstone ? first_tombstone : (node_type*)current_node;

					empty_node->set_valid();
					node_create_selector<KeyValueType, mapped_type, InsertEmplace>::create(empty_node, crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
					m_length++;
					return { iterator(m_data, m_data + get_capacity(), empty_node), true };
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
						current_node->set_valid();
						node_create_selector<KeyValueType, mapped_type, InsertEmplace>::create(current_node, crstl_forward(KeyType, key), crstl_forward(InsertEmplaceArgs, insert_emplace_args)...);
					}

					return { iterator(m_data, m_data + get_capacity(), current_node), Behavior == exists_behavior::assign };
				}
			
				current_node++;
				current_node = (current_node == end_node) ? m_data : current_node;
			
			} while(current_node != start_node);

			return { iterator(m_data, m_data + get_capacity(), nullptr), false };
		}

		template<typename KeyType>
		crstl_forceinline node_type* find_impl(const KeyType& key) const
		{
			const size_t hash_value = compute_hash_value(key);
			const size_t bucket_index = compute_bucket(hash_value);

			node_type* const data = (node_type*)m_data;
			node_type* const start_node = data + bucket_index;
			node_type* const end_node = data + get_capacity();
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

		node_type* begin_impl() const
		{
			if (empty())
			{
				return (node_type*)(m_data + get_capacity());
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

		static crstl_constexpr14 size_t compute_hash_value(const key_type& key)
		{
			size_t hash_value = hasher()(key);
			return hash_value;
		}
	};
};