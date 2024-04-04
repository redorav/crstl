#pragma once

#include "crstl/config.h"

#include "crstl/move_forward.h"

// crstl::pair
//
// Replacement for std::pair
//

crstl_module_export namespace crstl
{
	template<typename T1, typename T2>
	class pair
	{
	public:

		typedef T1 first_type;
		typedef T2 second_type;

		crstl_constexpr pair() crstl_noexcept : first(T1()), second(T2()) {}

		template<typename U1, typename U2>
		crstl_constexpr pair(const U1& first, const U2& second) crstl_noexcept : first(first), second(second) {}

		template<typename U1, typename U2>
		crstl_constexpr pair(U1&& first, U2&& second) crstl_noexcept : first(crstl_forward(U1, first)), second(crstl_forward(U2, second)) {}

		template<typename U1, typename U2>
		crstl_constexpr pair(const pair& other) crstl_noexcept : first(other.first), second(other.second) {}

		template<typename U1, typename U2>
		crstl_constexpr pair(pair<U1, U2>&& other) crstl_noexcept : first(crstl_forward(U1, other.first)), second(crstl_forward(U2, other.second)) {}

		template<typename U1, typename U2>
		pair<T1, T2>& operator = (const pair<U1, U2>& other)
		{
			first = other.first;
			second = other.second;
			return *this;
		}

		template<typename U1, typename U2>
		pair<T1, T2>& operator = (pair<U1, U2>&& other)
		{
			first = crstl_forward(U1, other.first);
			second = crstl_forward(U2, other.second);
			return *this;
		}

		crstl_constexpr bool operator == (const pair& other) const { return first == other.first && second == other.second; }
		crstl_constexpr bool operator != (const pair& other) const { return !(*this == other); }
		crstl_constexpr bool operator < (const pair& other) const { return first < other.first || (!(first < other.first) && second < other.second); }
		crstl_constexpr bool operator <= (const pair& other) const { return !(other < *this); }
		crstl_constexpr bool operator > (const pair& other) const { return other < *this; }
		crstl_constexpr bool operator >= (const pair& other) const { return !(*this < other); }

		T1 first;
		T2 second;
	};

	template<typename T1, typename T2>
	crstl_constexpr pair<T1, T2> make_pair(T1&& first, T2&& second)
	{
		return pair<T1, T2>(first, second);
	}
};