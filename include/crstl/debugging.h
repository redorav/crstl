#pragma once

#include "crstl/config.h"

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/debugging_win32.h"
#else
#error not implemented
#endif

crstl_module_export namespace crstl
{
	inline void breakpoint_if_debugger()
	{
		if (is_debugger_present())
		{
			breakpoint();
		}
	}
};