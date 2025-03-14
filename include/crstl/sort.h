#pragma once

#include "crstl/config.h"
#include "crstl/crstldef.h"
#include "crstl/move_forward.h"
#include "crstl/bit.h"
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

	// Map sort to quick_sort
	template<typename T>
	void sort(T* begin, T* end)
	{
		quick_sort(begin, end, less<>{});
	}
};