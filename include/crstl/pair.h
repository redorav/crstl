#pragma once

#include "config.h"

#include "type_utils.h"

namespace crstl
{
	template<typename T1, typename T2>
	class pair
	{
	public:

		typedef T1 first_type;
		typedef T2 second_type;

		crstl_constexpr pair() crstl_noexcept : first(T1()), second(T2()) {}
		crstl_constexpr pair(const T1& first, const T2& second) crstl_noexcept : first(first), second(second) {}
		crstl_constexpr pair(const T1& first, T2&& second) crstl_noexcept : first(first), second(crstl::move(second)) {}
		crstl_constexpr pair(T1&& first, const T2& second) crstl_noexcept : first(crstl::move(first)), second(second) {}
		crstl_constexpr pair(T1&& first, T2&& second) crstl_noexcept : first(crstl::move(first)), second(crstl::move(second)) {}

		template<typename O1, typename O2>
		crstl_constexpr pair(const O1& first, const O2& second) crstl_noexcept : first(first), second(second) {}

		template<typename O1, typename O2>
		crstl_constexpr pair(O1&& first, O2&& second) crstl_noexcept : first(crstl::forward<O1>(first)), second(crstl::forward<O2>(second)) {}

		template<typename O1, typename O2>
		crstl_constexpr pair(const pair<O1, O2>& other) crstl_noexcept : first(other.first), second(other.second) {}

		template<typename O1, typename O2>
		crstl_constexpr pair(pair<O1, O2>&& other) crstl_noexcept : first(crstl::forward<O1>(other.first)), second(crstl::forward<O2>(other.second)) {}

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