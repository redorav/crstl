#pragma once

#include "crstl/config.h"

#if defined(CRSTL_COMPILER_MSVC)
#include "crstl/platform/common_win32.h"

extern "C"
{
	void __debugbreak();

	__declspec(dllimport) crstl::BOOL IsDebuggerPresent();
};
#elif defined(CRSTL_OS_OSX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

crstl_module_export namespace crstl
{
	inline void breakpoint() crstl_noexcept
	{
#if defined(CRSTL_COMPILER_MSVC)
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
#elif defined(CRSTL_OS_OSX)
		// https://developer.apple.com/library/archive/qa/qa1361/_index.html

		// Initialize the flags so that, if sysctl fails for some reason, we get a predictable result
		struct kinfo_proc info;
		info.kp_proc.p_flag = 0;

		// Initialize mib, which tells sysctl the info we want, in this case we're looking for information about a specific process ID
		int mib[4];
		mib[0] = CTL_KERN;
		mib[1] = KERN_PROC;
		mib[2] = KERN_PROC_PID;
		mib[3] = getpid();

		size_t size = sizeof(info);
		int sysctl_return = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
		crstl_assert(sysctl_return == 0);

		// We're in the debugger if the P_TRACED flag is set
		return ((info.kp_proc.p_flag & P_TRACED) != 0);
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