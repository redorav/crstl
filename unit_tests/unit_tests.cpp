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

Dummy::Dummy(const Dummy& dummy)
{
	a = dummy.a;
	b = dummy.b;
	c = dummy.c;
	d = dummy.d;
	e = dummy.e;
	f = dummy.f;
	g = dummy.g;
	h = dummy.h;
	i = dummy.i;
	j = dummy.j;
	ptr = dummy.ptr;
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
	a = dummy.a;
	b = dummy.b;
	c = dummy.c;
	d = dummy.d;
	e = dummy.e;
	f = dummy.f;
	g = dummy.g;
	h = dummy.h;
	i = dummy.i;
	j = dummy.j;
	ptr = dummy.ptr;
	return *this;
}

Dummy::~Dummy()
{
	// Set to values that look uninitialized, for debugging
	a = -858993460;
	b = -107374176.0f;
}

bool Dummy::operator == (const Dummy& other) const
{
	bool isEqual =
		a == other.a &&
		b == other.b;

	return isEqual;
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
		(void)functionString;
		(void)lineNumber;
		(void)file;
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
void RunUnitTestsAssociative();
void RunUnitTestsBitset();
void RunUnitTestsDeque();
void RunUnitTestsFunction();
void RunUnitTestsPair();
void RunUnitTestsPath();
void RunUnitTestsProcess();
void RunUnitTestsSmartPtr();
void RunUnitTestsString();
void RunUnitTestsThread();
void RunUnitTestsTimer();
void RunUnitTestsVector();

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
	RunUnitTestsDeque();
	RunUnitTestsFunction();
	RunUnitTestsPair();
	RunUnitTestsPath();
	RunUnitTestsProcess();
	RunUnitTestsSmartPtr();
	RunUnitTestsString();
	RunUnitTestsThread();
	RunUnitTestsTimer();
	RunUnitTestsVector();
	
}

#if defined(__ANDROID__)
}
#endif