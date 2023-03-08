#pragma once

#include "config.h"

#include "crstldef.h"

namespace crstl
{
	// true_type/false_type

	template<typename T, T Value>
	struct integral_constant { static const T value = Value; };

	typedef integral_constant<bool, true> true_type;
	typedef integral_constant<bool, false> false_type;

	// remove_x

	template <typename T> struct remove_reference { typedef T type; };

	template <typename T> struct remove_reference<T&> { typedef T type; };

	template <typename T> struct remove_reference<T&&> { typedef T type; };

	template <typename T>           struct remove_const { typedef T type; };
	template <typename T>           struct remove_const<const T> { typedef T type; };
	template <typename T>           struct remove_const<const T[]> { typedef T type[]; };
	template <typename T, size_t N> struct remove_const<const T[N]> { typedef T type[N]; };

	template <typename T>           struct remove_volatile { typedef T type; };
	template <typename T>           struct remove_volatile<volatile T> { typedef T type; };
	template <typename T>           struct remove_volatile<volatile T[]> { typedef T type[]; };
	template <typename T, size_t N> struct remove_volatile<volatile T[N]> { typedef T type[N]; };

	template <typename T> struct remove_cv { typedef typename crstl::remove_volatile<typename crstl::remove_const<T>::type>::type type; };

	// enable_if

	template<bool Test, typename T = void> struct enable_if;
	template<typename T> struct enable_if<true, T> { typedef T type; };

	// conditional

	template<bool Test, typename T1, typename T2> struct conditional { typedef T2 type; }; // Type is T2 when Test is false
	template<typename T1, typename T2> struct conditional<true, T1, T2> { typedef T1 type; }; // Type is T1 when Test is true
	
	// is_array

	template<typename T> struct is_array : false_type {};
	template<typename T, size_t N> struct is_array<T[N]> : true_type {};
	template<typename T> struct is_array<T[]> : true_type {};

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
	crstl_constexpr static void swap(T& a, T& b) crstl_noexcept
	{
		T temp(crstl::move(a));
		a = crstl::move(b);
		b = crstl::move(temp);
	}

	// Swap for iterators
	template <class Iter1, class Iter2>
	crstl_constexpr void iter_swap(Iter1 left, Iter2 right) crstl_noexcept
	{
		swap(*left, *right);
	}

	// This swap is specific for arrays
	template<typename T, int N>
	crstl_constexpr void swap(T(&left)[N], T(&right)[N]) crstl_noexcept
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