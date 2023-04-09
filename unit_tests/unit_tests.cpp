#include "unit_tests.h"

#include <assert.h>
#include <stdio.h>

Dummy::Dummy() : a(4), b(5.0f), ptr(nullptr)
{

}

Dummy::Dummy(int a, float b) : a(a), b(b), ptr(nullptr)
{

}

Dummy::Dummy(int a, float b, int* ptr) : a(a), b(b), ptr(ptr)
{

}

Dummy::Dummy(const Dummy& d)
{
	memcpy(this, &d, sizeof(Dummy));
}

Dummy::Dummy(Dummy&& d) crstl_noexcept : a(d.a), b(d.b), ptr(d.ptr)
{
	d.ptr = (int*)(int)(0xDEADBEEF);
}

Dummy::Dummy(int a, float b, int c, int d, int e, int f, int g, int h, int i, int j) : a(a), b((float)b), c(c), d(d), e(e), f(f), g(g), h(h), i(i), j(j)
{

}

Dummy& Dummy::operator=(const Dummy& dummy)
{
	memcpy(this, &dummy, sizeof(Dummy));
	return *this;
}

Dummy::~Dummy()
{

}

void Dummy::PrintName()
{
	printf("Dummy\n");
}

void RefCountDummy::PrintName()
{
	printf("RefCountDummy\n");
}

RefCountDummy::~RefCountDummy()
{
	printf("RefCountDummy destroyed\n");
}

namespace crstl_unit
{
	void begin_test_impl(const char* name, const char* filename)
	{
		printf("%s, %s\n", name, filename);
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
		else
		{
			assert(false);
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