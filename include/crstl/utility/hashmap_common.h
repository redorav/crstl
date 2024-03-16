#pragma once

#include "crstl/config.h"

#define crstl_is_pow2(n) (!((n) & ((n) - 1)))

crstl_module_export namespace crstl
{
	// Compute which bucket a certain hash_value goes into, by taking into account whether it's a power of 2 or not

	template<bool IsPowerOfTwo = false>
	struct compute_bucket_function
	{
		static size_t compute_bucket(size_t hash_value, size_t bucket_count) { return hash_value % bucket_count; }
	};

	template<>
	struct compute_bucket_function<true>
	{
		static size_t compute_bucket(size_t hash_value, size_t bucket_count) { return hash_value & (bucket_count - 1); }
	};

	template<size_t BucketCount>
	size_t compute_bucket(size_t hash_value)
	{
		size_t bucket_index = compute_bucket_function<crstl_is_pow2(BucketCount)>::compute_bucket(hash_value, BucketCount);
		crstl_assert(bucket_index < BucketCount);
		return bucket_index;
	}

	// To be able to select between a const and a non-const object for const and non-const iterators
	template <bool Condition, class IsTrueType, class IsFalseType>
	struct hashmap_type_select { typedef IsTrueType type; };

	template <typename IsTrueType, class IsFalseType>
	struct hashmap_type_select<false, IsTrueType, IsFalseType> { typedef IsFalseType type; };

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
	template<typename KeyValueType, typename T, insert_emplace InsertEmplace>
	struct node_create_selector;

	// TODO Rework this to not pass NodeType but pass a pointer to key_value instead
	template<typename KeyValueType, typename T>
	struct node_create_selector<KeyValueType, T, insert_emplace::insert>
	{
		template<typename NodeType, typename KeyType, typename ValueType>
		crstl_forceinline static void create(NodeType* new_node, KeyType&& key, ValueType&& value)
		{
			crstl_placement_new((void*)&(new_node->key_value)) KeyValueType(crstl::forward<const KeyType>(key), crstl::forward<ValueType>(value));
		}
	};

	template<typename KeyValueType, typename T>
	struct node_create_selector<KeyValueType, T, insert_emplace::emplace>
	{
		template<typename NodeType, typename KeyType, typename... Args>
		crstl_forceinline static void create(NodeType* new_node, KeyType&& key, Args&&... args)
		{
			crstl_placement_new((void*)&(new_node->key_value)) KeyValueType(crstl::forward<const KeyType>(key), T(crstl::forward<Args>(args)...));
		}
	};
};