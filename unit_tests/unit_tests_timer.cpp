#include "unit_tests.h"

#include "crstl/timer.h"

#include <stdio.h>

void RunUnitTestsTimer()
{
	crstl::timer crTimer;

	crstl::time start_time = crTimer.elapsed();

	for (int i = 0; i < 100000; ++i)
	{

	}

	crstl::time end_time = crTimer.elapsed();

	crstl::time elapsed = end_time - start_time;

	printf("Timer Start: %f End: %f Elapsed: %f\n", start_time.milliseconds(), end_time.milliseconds(), elapsed.milliseconds());

	crstl::cycle_timer crCycleTimer;
	
	for (int i = 0; i < 100000; ++i)
	{

	}

	printf("Ticks elapsed: %llu\n", (long long unsigned int)crCycleTimer.elapsed());
}
