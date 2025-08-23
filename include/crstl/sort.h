#pragma once

#include "crstl/config.h"
#include "crstl/crstldef.h"
#include "crstl/move_forward.h"
#include "crstl/utility/cast.h"
#include "crstl/utility/memory_ops.h"

crstl_module_export namespace crstl
{
#if defined(CRSTL_QUICK_SORT_INSERTION_SIZE)
	static const size_t kQuicksortInsertionLimit = CRSTL_QUICK_SORT_INSERTION_SIZE;
#else
	static const size_t kQuicksortInsertionLimit = 32;
#endif

	template <typename T = void>
	struct less
	{
		crstl_constexpr14 bool operator()(const T& a, const T& b) const
		{
			return a < b;
		}
	};

	template<>
	struct less<void>
	{
		template<typename A, typename B>
		crstl_constexpr14 bool operator()(A&& a, B&& b) const
		{
			return crstl_forward(A, a) < crstl_forward(B, b);
		}
	};

	template<typename T>
	void swap(T& left, T& right)
	{
		T temp = crstl_move(left);
		left   = crstl_move(right);
		right  = crstl_move(temp);
	}

	// bubble_sort is the simplest possible sort. It starts from the beginning and checks the data set multiple times,
	// swapping as necessary. If there are no swap in one of the runs, the array is sorted. It is very slow on large
	// sets and insertion_sort is usually faster
	template<typename T, typename Compare>
	void bubble_sort(T* begin, T* end, Compare compare)
	{
		crstl_assert(begin < end);

		const size_t size = end - begin;

		bool swapped = true;

		while (swapped)
		{
			swapped = false;

			for (size_t i = 0; i < size - 1; ++i)
			{
				if (compare(begin[i + 1], begin[i]))
				{
					swap(begin[i], begin[i + 1]);
					swapped = true;
				}
			}
		}
	}

	template<typename T>
	void bubble_sort(T* begin, T* end)
	{
		bubble_sort(begin, end, less<>());
	}

	// insertion_sort takes an element and inserts it in the sorted position of an array. To accomplish that it checks 
	// for all elements until one is found that does not meet the criteria. On its way all elements are moved until we
	// find a place for the current element
	template<typename T, typename Compare>
	void insertion_sort(T* begin, T* end, Compare compare)
	{
		const size_t size = end - begin;

		for (size_t i = 1; i < size; ++i)
		{
			// Access the current element
			T current_element = crstl_move(begin[i]);

			size_t j = i;

			// Compare it walking backward
			for (; j > 0; --j)
			{
				T sort_element = crstl_move(begin[j - 1]);

				if (compare(current_element, sort_element))
				{
					begin[j] = crstl_move(sort_element);
				}
				else
				{
					break;
				}
			}

			// Write the last element
			begin[j] = crstl_move(current_element);
		}
	}

	template<typename T>
	void insertion_sort(T* begin, T* end)
	{
		insertion_sort(begin, end, less<>());
	}

	template <typename T, typename Compare>
	inline const T& median(const T& a, const T& b, const T& c, Compare compare)
	{
		if (compare(a, b))
		{
			if (compare(b, c))
				return b;
			else if (compare(a, c))
				return c;
			else
				return a;
		}
		else if (compare(a, c))
			return a;
		else if (compare(b, c))
			return c;
		return b;
	}

	template<typename T, typename Compare>
	void quick_sort(T* begin, T* end, Compare compare)
	{
		crstl_assert(begin <= end);

		const size_t size = end - begin;

		if (size > kQuicksortInsertionLimit)
		{
			// Select pivot from the middle of the array. Median of 3 has some overhead, might be better in some cases
			// if we can find a really fast way of selecting it (e.g. branchless for integers)
			T pivot = begin[size >> 1];

			// Distribute items for this range. Start from the left and start swapping with the elements at the end
			// when items are on the wrong side of the pivot
			int64_t left_index = -1;
			int64_t right_index = size;

			while (true)
			{
				// We don't range compare here because whether the pivot is the maximum or minimum element,
				// this is guaranteed to compare with the pivot itself at some point, by construction
				while (compare(begin[++left_index], pivot));
				while (compare(pivot, begin[--right_index]));

				if (left_index < right_index)
				{
					swap(begin[left_index], begin[right_index]);
				}
				else
				{
					break;
				}
			}

			// Call recursively for partitions
			quick_sort(begin, begin + left_index, compare);
			quick_sort(begin + left_index, end, compare);
		}
		else
		{
			insertion_sort(begin, end, compare);
		}
	}

	template<typename T>
	void quick_sort(T* begin, T* end)
	{
		quick_sort(begin, end, less<>{});
	}

	// Radix Sort
	//
	// https://probablydance.com/2016/12/02/investigating-radix-sort/
	// http://stereopsis.com/radix.html
	//
	// This version has a couple of optimizations seen elsewhere
	// 
	// 1. Computes counts and offsets upfront to reduce the number of loops through the data. Other implementations handle
	//    it cleverly by computing it for the next iteration to reduce stack usage but simplicity was favored in our case
	// 2. Keeps track of whether a single bucket was used, to avoid sorting parts of the word that are the same. This
	//    behavior happens when using e.g. smaller numbers or aligned pointers

	template<typename T>
	struct radix_sort_key
	{
		typedef decltype(T::m_key) radix_type;

		static radix_type extract(const T& item)
		{
			return item.m_key;
		}
	};

	template<> struct radix_sort_key<int8_t>   { typedef int8_t   radix_type; static int8_t   extract(int8_t item)   { return item; } };
	template<> struct radix_sort_key<uint8_t>  { typedef uint8_t  radix_type; static uint8_t  extract(uint8_t item)  { return item; } };
	template<> struct radix_sort_key<int16_t>  { typedef int16_t  radix_type; static int16_t  extract(int16_t item)  { return item; } };
	template<> struct radix_sort_key<uint16_t> { typedef uint16_t radix_type; static uint16_t extract(uint16_t item) { return item; } };
	template<> struct radix_sort_key<int32_t>  { typedef int32_t  radix_type; static int32_t  extract(int32_t item)  { return item; } };
	template<> struct radix_sort_key<uint32_t> { typedef uint32_t radix_type; static uint32_t extract(uint32_t item) { return item; } };
	template<> struct radix_sort_key<int64_t>  { typedef int64_t  radix_type; static int64_t  extract(int64_t item)  { return item; } };
	template<> struct radix_sort_key<uint64_t> { typedef uint64_t radix_type; static uint64_t extract(uint64_t item) { return item; } };

	template<> struct radix_sort_key<float>
	{
		typedef uint32_t radix_type;
		
		static uint32_t extract(float item)
		{
			uint32_t u_item = union_cast<uint32_t>(item);
			uint32_t mask = -int32_t(u_item >> 31) | 0x80000000;
			return u_item ^ mask;
		}
	};

	template<> struct radix_sort_key<double>
	{
		typedef uint64_t radix_type;
		
		static uint64_t extract(double item)
		{
			uint64_t u_item = union_cast<uint64_t>(item);
			uint64_t mask = -int64_t(u_item >> 63) | 0x8000000000000000;
			return u_item ^ mask;
		}
	};

	template<typename T>
	void radix_sort(T* begin, T* end, T* temp_buffer = nullptr)
	{
		typedef typename radix_sort_key<T>::radix_type radix_type;

		const size_t size = end - begin;

		if (size > 1)
		{
			bool free_memory = false;

			// Allocate temporary memory according to our needs
			T* temp_memory = nullptr;

			if (temp_buffer)
			{
				temp_memory = temp_buffer;
			}
			else
			{
				temp_memory = new T[size];
				free_memory = true;
			}

			static const size_t kBitsPerPass = 8;
			static const size_t kBitCount    = sizeof(radix_type) * 8;
			static const size_t kPassCount   = kBitCount / kBitsPerPass;
			static const size_t kBucketCount = size_t(1) << kBitsPerPass;
			static const size_t kBitmask     = kBucketCount - size_t(1);

			static_assert((kBitCount & kBitsPerPass) == 0, "bit_count has to be a multiple of bits_per_pass");

			uint32_t bucket[kBucketCount * kPassCount] = {};
			uint32_t bucket_offsets[kBucketCount * kPassCount]; // Don't initialize

			uint32_t bucket_sums[kPassCount] = {};
			uint8_t  bucket_allequal[kPassCount] = {};

			T* input = begin;
			T* output = temp_memory;

			size_t pass_count = 0;

			radix_type candidate0 = radix_sort_key<T>::extract(input[0]);

			// Initialize all buckets and compute prefix sum for all passes. This is much faster than computing as we go since we reduce
			// the number of times we loop through the whole data set
			for (size_t i = 0; i < size; ++i)
			{
				radix_type candidate = radix_sort_key<T>::extract(input[i]);

				for (size_t pass_index = 0; pass_index < kPassCount; ++pass_index)
				{
					size_t pass_shift = pass_index * kBitsPerPass;
					uint32_t bucket_entry = (candidate >> pass_shift) & kBitmask;
					bucket_sums[pass_index] += bucket_entry;
					bucket[pass_index * kBucketCount + bucket_entry]++;
				}
			}

			for (size_t pass_index = 0; pass_index < kPassCount; ++pass_index)
			{
				size_t pass_shift = pass_index * kBitsPerPass;
				uint32_t bucket_entry0 = (candidate0 >> pass_shift) & kBitmask;
				bool all_buckets_equal = (bucket_sums[pass_index] / 255) == bucket_entry0;
				bucket_allequal[pass_index] = all_buckets_equal;

				if (!all_buckets_equal)
				{
					bucket_offsets[pass_index * kBucketCount] = 0;
					for (size_t i = 1; i < kBucketCount; ++i)
					{
						bucket_offsets[pass_index * kBucketCount + i] = bucket_offsets[pass_index * kBucketCount + i - 1] + bucket[pass_index * kBucketCount + i - 1];
					}
				}
			}

			for (size_t pass_index = 0; pass_index < kPassCount; ++pass_index)
			{
				size_t pass_shift = pass_index * kBitsPerPass;
				bool all_buckets_equal = bucket_allequal[pass_index];
				if (!all_buckets_equal)
				{
					for (size_t i = 0; i < size; ++i)
					{
						radix_type candidate = radix_sort_key<T>::extract(input[i]);
						size_t bucket_entry = (size_t)((candidate >> pass_shift) & kBitmask);
						size_t bucket_offset = bucket_offsets[pass_index * kBucketCount + bucket_entry]++;
						output[bucket_offset] = crstl_move(input[i]);
					}

					// Swap input and output
					T* temp = input;
					input = output;
					output = temp;

					pass_count++;
				}
			}

			// If the last output was written to the temp memory we need to copy it back out
			if (pass_count & 1)
			{
				for (size_t i = 0; i < size; ++i)
				{
					begin[i] = crstl_move(temp_memory[i]);
				}
			}

			if (free_memory)
			{
				delete[] temp_memory;
			}
		}
	}
	// Map sort to quick_sort
	template<typename T>
	void sort(T* begin, T* end)
	{
		quick_sort(begin, end, less<>{});
	}
};