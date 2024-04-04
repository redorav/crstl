#pragma once

// These are so commonly used without anything else that they warrant their own header

crstl_module_export namespace crstl
{
	template <typename T> struct remove_reference { typedef T type; };

	template <typename T> struct remove_reference<T&> { typedef T type; };

	template <typename T> struct remove_reference<T&&> { typedef T type; };

	template<typename T> struct is_lvalue_reference { static const bool value = false; };

	template<typename T> struct is_lvalue_reference<T&> { static const bool value = true; };

	template <typename T>
	crstl_constexpr T&& forward(typename crstl::remove_reference<T>::type& x) crstl_noexcept
	{
		return static_cast<T&&>(x);
	}

	template <typename T>
	crstl_constexpr T&& forward(typename crstl::remove_reference<T>::type&& x) crstl_noexcept
	{
		static_assert(!crstl::is_lvalue_reference<T>::value, "T is not an lvalue reference");
		return static_cast<T&&>(x);
	}

	template <typename T>
	crstl_constexpr typename crstl::remove_reference<T>::type&& move(T&& x) crstl_noexcept
	{
		return static_cast<typename crstl::remove_reference<decltype(x)>::type&&>(x);
	}
};

// https://www.foonathan.net/2020/09/move-forward/

// Use inside the library for faster code in debug builds
#define crstl_move(x) static_cast<typename crstl::remove_reference<decltype(x)>::type&&>(x)

#define crstl_forward(T, x) static_cast<T&&>(x)