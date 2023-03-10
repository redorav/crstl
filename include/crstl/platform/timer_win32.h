union _LARGE_INTEGER;

extern "C"
{
	int QueryPerformanceFrequency(_LARGE_INTEGER* lpFrequency);
	int QueryPerformanceCounter(_LARGE_INTEGER* lpPerformanceCount);

	unsigned __int64 __rdtsc();
}

#pragma intrinsic(__rdtsc)

namespace crstl
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