#include <time.h>

// https://gist.github.com/savanovich/f07eda9dba9300eb9ccf
static inline crstl::int64_t rdtsc_start(void)
{
	unsigned a, d; 
	asm volatile("cpuid" ::: "%rax", "%rbx", "%rcx", "%rdx");
	asm volatile("rdtsc" : "=a" (a), "=d" (d)); 
	return ((unsigned long)a) | (((unsigned long)d) << 32); 
}

static inline crstl::int64_t rdtsc_end(void)
{
	unsigned a, d; 
	asm volatile("rdtscp" : "=a" (a), "=d" (d)); 
	asm volatile("cpuid" ::: "%rax", "%rbx", "%rcx", "%rdx");
	return ((unsigned long)a) | (((unsigned long)d) << 32); 
}

namespace crstl
{
	crstl_constexpr uint64_t ticks_per_second()
	{
		return 1000000000;
	}

	inline uint64_t current_ticks()
	{
		timespec time;
    		clock_gettime(CLOCK_REALTIME, &time);
		return time.tv_sec * 1000000000LL + time.tv_nsec;
	}

	inline uint64_t begin_cycle_count()
	{
		return rdtsc_start();
	}
	
	inline uint64_t end_cycle_count()
	{
		return rdtsc_end();
	}
};
