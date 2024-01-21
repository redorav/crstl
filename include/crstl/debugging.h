#pragma once

#include "crstl/config.h"

#if defined(CRSTL_COMPILER_MSVC)
#include "crstl/platform/common_win32.h"

extern "C"
{
	void __debugbreak();

	__declspec(dllimport) crstl::BOOL IsDebuggerPresent();
};
#endif

crstl_module_export namespace crstl
{
	inline void breakpoint() crstl_noexcept
	{
#if defined(CRSTL_COMPILER_MSVC) || defined(CRSTL_COMPILER_INTEL)
		__debugbreak();
#elif defined(__has_builtin) && __has_builtin(__builtin_debugtrap) // Mostly the Clang compiler
		__builtin_debugtrap();
#elif defined(CRSTL_ARCH_X86)
		__asm__ volatile("int $0x03");
#elif defined(CRSTL_ARCH_ARM32)
		__asm__ volatile("udf #0xfe");
#elif defined(CRSTL_ARCH_ARM64)
		__asm__ volatile("brk #0xf000");
#elif defined(CRSTL_ARCH_POWERPC)
		__asm__ volatile(".4byte 0x7d821008");
#else
		#error not implemented
#endif
	}

	inline bool is_debugger_present() crstl_noexcept
	{
#if defined(CRSTL_COMPILER_MSVC)
		return IsDebuggerPresent();
#else
		return 0;
#endif
	}

	inline void breakpoint_if_debugger()
	{
		if (is_debugger_present())
		{
			breakpoint();
		}
	}
};