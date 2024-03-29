#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

// crstl::allocator
//
// Replacement for std::allocator
//

crstl_module_export namespace crstl
{
	class allocator
	{
	public:

		typedef size_t size_type;

		crstl_nodiscard void* allocate(size_type size_bytes) const crstl_noexcept
		{
			return ::operator new(size_bytes);
		}

		crstl_nodiscard void* allocate_at_least(size_type size_bytes) const crstl_noexcept
		{
			return ::operator new(size_bytes);
		}

		void deallocate(void* p, size_type /*size_bytes*/ ) const crstl_noexcept
		{
			::operator delete(p);
		}
	};
};