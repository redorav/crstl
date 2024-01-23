#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/thread.h"
#include "crstl/critical_section.h"
#include "crstl/tuple.h"

#include "crstl/timer.h" // For sleep timings
#endif

#include <stdio.h>

int ThreadFunctionWithParams(int a, int b, int c)
{
	int sum = a + b + c;
	printf("ThreadFunctionWithParams %i\n", sum);
	return sum;
}

void RunUnitTestsThread()
{
	printf("RunUnitTestsThread\n");

	int i = 42;
	int a = 2;
	float b = 3.0f;
	char c = 'a';

	crstl::tuple<int, float, char> crTupleConstructorMove(2, 3.0f, 'a');

	crstl::tuple<int, float, char> crTupleConstructorConstRef(2, 3.0f, c); crstl_unused(crTupleConstructorConstRef);

	crstl::tuple<int, float, char> crTupleBracketsMove = { 2, 3.0f, 'a' }; crstl_unused(crTupleBracketsMove);

	crstl::tuple<int, float, char> crTupleBracketsConstRef = { a, b, 'l' }; crstl_unused(crTupleBracketsConstRef);
	
	crstl::tuple<int, float, char> crTupleCopy(crTupleConstructorMove);

	size_t tuple_size = crTupleConstructorMove.size(); crstl_unused(tuple_size);

	float f_get = crstl::get<1>(crTupleBracketsMove); crstl_unused(f_get);

	char c_get = crstl::get<2>(crTupleBracketsConstRef); crstl_unused(c_get);
	
	int i_get = crTupleCopy.get<0>(); crstl_unused(i_get);

	crstl::thread_parameters params;
	
	params.debug_name = "Thread 1";
	crstl::thread crThread1(params, []()
	{
		printf("Thread 1\n");
	});

	params.debug_name = "Lambda Thread With Capture";
	crstl::thread crThread2(params, [i]()
	{
		crstl_unused(i);
		printf("Lambda Thread With Capture\n");
	});

	params.debug_name = "Lambda Thread With Capture 2";
	crstl::thread crThread3(params, [&i]()
	{
		crstl_unused(i);
		printf("Lambda Thread With Capture 2\n");
	});

	params.debug_name = "ThreadFunctionWithParams";
	crstl::thread crThread4(params, &ThreadFunctionWithParams, 1, 2, 3);

	crstl::thread crThread5 = crstl::thread(params, &ThreadFunctionWithParams, 1, 2, 3);

	crstl::timer yieldTimer(true);
	crstl::this_thread::yield();
	printf("Yielded for %fms\n", yieldTimer.elapsed().milliseconds());

	printf("Sleeping...\n");
	crstl::timer leTimer(true);
	crstl::this_thread::sleep_for(1000);
	printf("Slept for %fms\n", leTimer.elapsed().milliseconds());

	crThread1.join();
	crThread2.join();
	crThread3.join();
	crThread4.join();
	crThread5.join();
}