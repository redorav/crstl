#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#if defined(CRSTL_OS_MACOS)
#include <mach/mach_time.h>
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_ANDROID)
#include <time.h>
#endif

#if defined(CRSTL_COMPILER_MSVC)

#include "crstl/platform/common_win32.h"

extern "C"
{
	__declspec(dllimport) int QueryPerformanceFrequency(_LARGE_INTEGER* lpFrequency);
	__declspec(dllimport) int QueryPerformanceCounter(_LARGE_INTEGER* lpPerformanceCount);

#if defined(CRSTL_ARCH_X86)
	unsigned __int64 __rdtsc();
	#pragma intrinsic(__rdtsc)
#elif defined(CRSTL_ARCH_ARM)

	#define CRSTL_ARM64_SYSREG(op0, op1, crn, crm, op2) \
        ( ((op0 & 1) << 14) | \
          ((op1 & 7) << 11) | \
          ((crn & 15) << 7) | \
          ((crm & 15) << 3) | \
          ((op2 & 7) << 0) )

	#define CRSTL_ARM64_PMCCNTR_EL0       CRSTL_ARM64_SYSREG(3,3, 9,13,0)  // Cycle Count Register [CP15_PMCCNTR]
	#define CRSTL_ARM64_CNTVCT            CRSTL_ARM64_SYSREG(3,3,14, 0,2)  // Generic Timer counter register

	#if defined(CRSTL_ARCH_ARM64)

	__int64 _ReadStatusReg(int);

	#elif defined(CRSTL_ARCH_ARM32)

	int _ReadStatusReg(int);

	#endif
#endif
};

#endif

#if defined(CRSTL_ARCH_X86)

extern "C"
{
	void _mm_lfence();
};

#elif defined(CRSTL_ARCH_ARM)

namespace crstl
{
	namespace detail
	{
		typedef enum
		{
			_ARM64_BARRIER_SY    = 0xF,
			_ARM64_BARRIER_ST    = 0xE,
			_ARM64_BARRIER_LD    = 0xD,
			_ARM64_BARRIER_ISH   = 0xB,
			_ARM64_BARRIER_ISHST = 0xA,
			_ARM64_BARRIER_ISHLD = 0x9,
			_ARM64_BARRIER_NSH   = 0x7,
			_ARM64_BARRIER_NSHST = 0x6,
			_ARM64_BARRIER_NSHLD = 0x5,
			_ARM64_BARRIER_OSH   = 0x3,
			_ARM64_BARRIER_OSHST = 0x2,
			_ARM64_BARRIER_OSHLD = 0x1
		}
		_ARM64INTR_BARRIER_TYPE;

		typedef enum
		{
			_ARM_BARRIER_SY    = 0xF,
			_ARM_BARRIER_ST    = 0xE,
			_ARM_BARRIER_ISH   = 0xB,
			_ARM_BARRIER_ISHST = 0xA,
			_ARM_BARRIER_NSH   = 0x7,
			_ARM_BARRIER_NSHST = 0x6,
			_ARM_BARRIER_OSH   = 0x3,
			_ARM_BARRIER_OSHST = 0x2
		}
		_ARMINTR_BARRIER_TYPE;
	};
};

extern "C"
{
	void __dmb(unsigned int _Type);
};

#endif

crstl_module_export namespace crstl
{
	inline uint64_t get_cycle_count()
	{
#if defined(CRSTL_OS_MACOS)
		return mach_absolute_time();
#elif defined(CRSTL_COMPILER_MSVC)

	// Inline assembly is not supported in MSVC so do our best to use the available intrinsics here
	#if defined(CRSTL_ARCH_X86)
		return __rdtsc();
	#elif defined(CRSTL_ARCH_ARM64)
		return _ReadStatusReg(CRSTL_ARM64_CNTVCT);
	#elif defined(CRSTL_ARCH_ARM32)
		return 0; // _ReadStatusReg(CRSTL_ARM64_PMCCNTR_EL0);
	#endif

#elif defined(CRSTL_ARCH_X86_32)
		uint64_t ret;
		__asm__ volatile("rdtsc" : "=A"(ret));
		return ret;
#elif defined(CRSTL_ARCH_X86_64)
		uint64_t low, high;
		__asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
		return (high << 32) | low;
#elif defined(CRSTL_ARCH_ARM64)
		uint64_t cntvct;
		asm volatile("mrs %0, cntvct_el0" : "=r"(cntvct));
		return cntvct;
#elif defined(CRSTL_ARCH_ARM32)
		uint64_t pmccntr;
		asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
		return pmccntr;
#else
		return 0;
#endif
	}

	inline void serializing_instruction()
	{
#if defined(CRSTL_ARCH_X86)
		_mm_lfence();
#elif defined(CRSTL_ARCH_ARM64)
		__dmb(detail::_ARM64_BARRIER_ISHLD);
#elif defined(CRSTL_ARCH_ARM32)
		__dmb(detail::_ARM_BARRIER_ISH);
#endif
	}

#if defined(CRSTL_OS_WINDOWS)

	inline double ticks_to_seconds()
	{
		long long ticksPerSecond;
		QueryPerformanceFrequency((_LARGE_INTEGER*)&ticksPerSecond);
		return 1.0 / (double)ticksPerSecond;
	}

	inline uint64_t current_ticks()
	{
		long long ticks;
		QueryPerformanceCounter((_LARGE_INTEGER*)&ticks);
		return (uint64_t)ticks;
	}

#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_ANDROID)

	crstl_constexpr double ticks_to_seconds()
	{
		return 1.0 / 1000000000.0;
	}

	inline uint64_t current_ticks()
	{
		timespec time;
		clock_gettime(CLOCK_REALTIME, &time);
		return (uint64_t)(time.tv_sec * 1000000000LL + time.tv_nsec);
	}

#elif defined(CRSTL_OS_MACOS)

	// https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x

	inline double ticks_to_seconds()
	{
		mach_timebase_info_data_t timebase;
		mach_timebase_info(&timebase);

		return (double)timebase.numer / ((double)timebase.denom);
	}

	inline uint64_t current_ticks()
	{
		return mach_absolute_time();
	}

#endif

	inline uint64_t begin_cycle_count()
	{
		return get_cycle_count();
	}

	inline uint64_t end_cycle_count()
	{
		return get_cycle_count();
	}
};