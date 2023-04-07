#pragma once

// We use this to disable some unit tests on old compilers just so they compile
// and run the rest of the tests. Put here so we don't accidentally use it in 
// the actual library
#if !(defined(_MSC_VER) && _MSC_VER < 1900)
#define CRSTL_UNIT_RANGED_FOR
#define CRSTL_UNIT_UNICODE_LITERALS

// Using curly brackets for initializing vectors, etc
#define CRSTL_UNIT_UNIFORM_INITIALIZATION
#endif

#if defined(__cpp_modules)

	#define CRSTL_UNIT_MODULES

#endif

#define crstl_unit_unused(x) (void)x

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/intrusive_ptr.h"

#include "stdio.h"

// Dummy structure with all the necessary constructors and operators to test different containers
class Dummy : public crstl::intrusive_ptr_interface_delete
{
public:

	Dummy() : a(4), b(5.0f), ptr(nullptr)
	{
		
	}

	Dummy(int a, float b) : a(a), b(b), ptr(nullptr)
	{

	}

	Dummy(int a, float b, int* ptr) : a(a), b(b), ptr(ptr)
	{
		
	}

	// Copy constructor
	Dummy(const Dummy& d)
	{
		memcpy(this, &d, sizeof(Dummy));
	}

	Dummy(Dummy&& d) crstl_noexcept : a(d.a), b(d.b), ptr(d.ptr)
	{
		d.ptr = (int*)(int)(0xDEADBEEF);
	}

	// For testing emplace()
	Dummy(int a, float b, int c, int d, int e, int f, int g, int h, int i, int j)
		: a(a), b((float)b), c(c), d(d), e(e), f(f), g(g), h(h), i(i), j(j) {}

	Dummy& operator = (const Dummy& dummy)
	{
		memcpy(this, &dummy, sizeof(Dummy));
		return *this;
	}

	~Dummy()
	{

	}

	void PrintName()
	{
		printf("Dummy\n");
	}

	int a;
	float b;
	int c;
	int d;
	int e;
	int f;
	int g;
	int h;
	int i;
	int j;

	int* ptr;
};

class DummyChild : public Dummy
{
public:

	int k;
};

class DummyOther
{
public:

	int k;
};

class RefCountDummy : public crstl::intrusive_ptr_interface_delete
{
public:

	void PrintName()
	{
		printf("RefCountDummy\n");
	}

	~RefCountDummy()
	{
		printf("RefCountDummy destroyed\n");
	}
};

namespace crstl_unit
{
	void begin_test_impl(const char* name);

	void end_test();

	#define crstl_check(x) \
		begin_test_case(#x, __LINE__, __FILE__); \
		check_condition(x); \
		end_test_case();

	#define begin_test(x)
	
	void begin_test_case(const char* functionString, int lineNumber, const char* file);

	void check_condition(bool condition);

	void end_test_case();
};