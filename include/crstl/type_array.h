#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/type_builtins.h"

crstl_module_export namespace crstl
{
	// is_array

	template<typename T> struct is_array { static const bool value = false; };
	template<typename T, size_t N> struct is_array<T[N]> { static const bool value = true; };
	template<typename T> struct is_array<T[]> { static const bool value = true; };

	// remove_extent: removes array extents

	template <typename T> struct remove_extent { typedef T type; };
	template <typename T, size_t N> struct remove_extent<T[N]> { typedef T type; };
	template <typename T> struct remove_extent<T[]> { typedef T type; };

	template<typename T, size_t N>
	crstl_constexpr size_t array_size(const T(&)[N]) crstl_noexcept
	{
		return N;
	}
};