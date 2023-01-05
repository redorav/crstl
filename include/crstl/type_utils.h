#pragma once

#include "config.h"

namespace crstl
{
	template <typename T> struct remove_reference { typedef T type; };

	template <typename T> struct remove_reference<T&> { typedef T type; };

	template <typename T> struct remove_reference<T&&> { typedef T type; };


	template <typename T>
	crstl_constexpr T&& forward(typename crstl::remove_reference<T>::type& x) crstl_noexcept
	{
		return static_cast<T&&>(x);
	}

	template <typename T>
	crstl_constexpr typename crstl::remove_reference<T>::type&& move(T&& x) crstl_noexcept
	{
		return static_cast<typename crstl::remove_reference<T>::type&&>(x);
	}

	// Swap for standard values
	template <typename T>
	crstl_constexpr static void swap(T& a, T& b)
	{
		T temp(crstl::move(a));
		a = crstl::move(b);
		b = crstl::move(temp);
	}

	template <class Iter1, class Iter2>
	crstl_constexpr void iter_swap(Iter1 left, Iter2 right)
	{
		swap(*left, *right);
	}

	// This swap is specific for arrays
	template<typename T, int N>
	crstl_constexpr void swap(T(&left)[N], T(&right)[N])
	{
		if (&left != &right)
		{
			T* iterLeft = left;
			T* endLeft = left + N;
			T* iterRight = right;
			while (iterLeft != endLeft)
			{
				crstl::iter_swap(iterLeft, iterRight); // Swap the pointers
				++iterLeft;
				++iterRight;
			}
		}
	}
};