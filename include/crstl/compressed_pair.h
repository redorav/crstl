#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

// crstl::compressed_pair
//
// Implements empty base class optimization for a pair of types 

crstl_module_export namespace crstl
{
	template<typename T1, typename T2, bool firstEmpty, bool secondEmpty>
	struct compressed_pair_base {};

	template<typename T1, typename T2>
	struct compressed_pair_base<T1, T2, true, false> : private T1
	{
		T1& first() { return *this; }
		const T1& first() const { return *this; }
		T2& second() { return m_second; }
		const T2& second() const { return m_second; }

		T2 m_second;
	};

	template<typename T1, typename T2>
	struct compressed_pair_base<T1, T2, false, true> : private T2
	{
		T1& first() { return m_first; }
		const T1& first() const { return m_first; }
		T2& second() { return *this; }
		const T2& second() const { return *this; }

		T1 m_first;
	};

	template<typename T1, typename T2>
	struct compressed_pair_base<T1, T2, false, false>
	{
		T1& first() { return m_first; }
		const T1& first() const { return m_first; }
		T2& second() { return m_second; }
		const T2& second() const { return m_second; }

		T1 m_first;
		T2 m_second;
	};

	template<typename T1, typename T2>
	struct compressed_pair_base<T1, T2, true, true> : private T1, private T2
	{
		T1& first() { return *this; }
		const T1& first() const { return *this; }
		T2& second() { return *this; }
		const T2& second() const { return *this; }
	};

	template<typename T1, typename T2>
	struct compressed_pair : compressed_pair_base<T1, T2, crstl_is_empty(T1), crstl_is_empty(T2)>
	{
		typedef compressed_pair_base<T1, T2, crstl_is_empty(T1), crstl_is_empty(T2)> base;

		T1& first() { return base::first(); }
		const T1& first() const { return base::first(); }
		T2& second() { return base::second(); }
		const T2& second() const { return base::second(); }
	};
};