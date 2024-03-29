#pragma once

#include "crstl/config.h"

// Create our own placement new to avoid including header <new> which is quite heavy
// We pass in a dummy parameter to disambiguate from the global placement new. The
// macro is there just for convenience
crstl_module_export namespace crstl
{
	enum class placement_new
	{
		dummy
	};
};

crstl_module_export inline void* operator new (crstl::size_t, crstl::placement_new, void* ptr) { return ptr; }
crstl_module_export inline void operator delete (void*, crstl::placement_new, void*) crstl_noexcept {}

#define crstl_placement_new(x) ::new(crstl::placement_new::dummy, x)