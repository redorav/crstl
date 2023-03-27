#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

namespace crstl
{
	template<typename T>
	class allocator
	{
	public:

		typedef T value_type;
		typedef size_t size_type;

		crstl_nodiscard crstl_constexpr T* allocate(size_type n) const crstl_noexcept
		{
			return (T*)::operator new(n * sizeof(T));
		}

		crstl_nodiscard crstl_constexpr T* allocate_at_least(size_type n) const crstl_noexcept
		{
			return (T*)::operator new(n * sizeof(T));
		}

		crstl_constexpr void deallocate(T* p, size_type /*n*/ ) const crstl_noexcept
		{
			::operator delete((void*)p);
		}
	};
};