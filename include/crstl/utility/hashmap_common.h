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
};