#pragma once

#include "crstl/config.h"

#define crstl_is_pow2(n) (!(n & (n - 1)))

crstl_module_export namespace crstl
{
	// Compute which bucket a certain hash_value goes into, by taking into account whether it's a power of 2 or not

	template<bool IsPowerOfTwo = false>
	struct compute_bucket_function
	{
		static size_t compute_bucket(size_t hash_value, size_t BucketCount) { return hash_value % BucketCount; }
	};

	template<>
	struct compute_bucket_function<true>
	{
		static size_t compute_bucket(size_t hash_value, size_t BucketCount) { return hash_value & (BucketCount - 1); }
	};

	template<size_t BucketCount>
	size_t compute_bucket(size_t hash_value)
	{
		size_t bucket_index = compute_bucket_function<crstl_is_pow2(BucketCount)>::compute_bucket(hash_value, BucketCount);
		crstl_assert(bucket_index < BucketCount);
		return bucket_index;
	}
};