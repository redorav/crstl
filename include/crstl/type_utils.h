#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/type_builtins.h"

crstl_module_export namespace crstl
{
	// true_type/false_type

	template<typename T, T Value>
	struct integral_constant { static const T value = Value; };

	typedef integral_constant<bool, true> true_type;
	typedef integral_constant<bool, false> false_type;

	template <bool B> struct bool_constant : public integral_constant<bool, B> {};

	// remove_cv

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

	// is_same
	template<typename, typename> struct is_same : false_type {};
	template<typename T> struct is_same<T, T> : true_type {};

	template <typename T> struct is_empty : bool_constant<crstl_is_empty(T)> {};
};