#pragma once

#include "crstl/crstldef.h"

union _LARGE_INTEGER;

extern "C"
{
	__declspec(dllimport) int QueryPerformanceFrequency(_LARGE_INTEGER* lpFrequency);
	__declspec(dllimport) int QueryPerformanceCounter(_LARGE_INTEGER* lpPerformanceCount);

	unsigned __int64 __rdtsc();
}

#pragma intrinsic(__rdtsc)

crstl_module_export namespace crstl
{
	inline uint64_t ticks_per_second()
	{
		long long ticksPerSecond;
		QueryPerformanceFrequency((_LARGE_INTEGER*)&ticksPerSecond);
		return (uint64_t)ticksPerSecond;
	}

	inline uint64_t current_ticks()
	{
		long long ticks;
		QueryPerformanceCounter((_LARGE_INTEGER*)&ticks);
		return (uint64_t)ticks;
	}

	inline uint64_t begin_cycle_count()
	{
		return __rdtsc();
	}

	inline uint64_t end_cycle_count()
	{
		return __rdtsc();
	}
};