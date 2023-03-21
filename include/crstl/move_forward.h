#pragma once

// These are so commonly used without anything else that they warrant their own header

crstl_module_export namespace crstl
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
};