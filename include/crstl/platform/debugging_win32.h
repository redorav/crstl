#pragma once

#include "common_win32.h"

extern "C"
{
	void __debugbreak();

	__declspec(dllimport) crstl::BOOL IsDebuggerPresent();
};

crstl_module_export namespace crstl
{
	inline void breakpoint() crstl_noexcept
	{
		__debugbreak();
	}

	inline bool is_debugger_present() crstl_noexcept
	{
		return IsDebuggerPresent();
	}
};