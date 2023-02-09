#include <assert.h>
#include <stdio.h>

namespace crstl_unit
{
	void begin_test(const char* name)
	{
		printf("%s\n", name);
	}

	void end_test()
	{

	}

	
	void begin_test_case(const char* functionString, int lineNumber, const char* file)
	{

	}

	void check_condition(bool condition)
	{
		if (condition)
		{
		}
	}

	void end_test_case()
	{

	}
}

void RunUnitTestsArray();
void RunUnitTestsBitset();
void RunUnitTestsString();
void RunUnitTestsVector();
void RunUnitTestsFunction();
void RunUnitTestsSmartPtr();
void RunUnitTestsPair();
void RunUnitTestsAssociative();
void RunUnitTestsTimer();

#if defined(__ANDROID__)

#if defined(__cplusplus)
extern "C"
{
#endif

	void android_main(struct android_app* app)

#else

	int main()

#endif
{
	RunUnitTestsArray();
	RunUnitTestsBitset();
	RunUnitTestsString();
	RunUnitTestsVector();
	RunUnitTestsFunction();
	RunUnitTestsSmartPtr();
	RunUnitTestsPair();
	RunUnitTestsAssociative();
	RunUnitTestsTimer();
}

#if defined(__ANDROID__)
}
#endif