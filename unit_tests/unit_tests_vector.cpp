#include "unit_tests.h"

#include "crstl/fixed_vector.h"
#include "crstl/span.h"

#include <vector>
#include <stdio.h>

#if crstl_cppversion >= 202002L
#include <span>
#endif

struct Dummy
{
	Dummy() : a(4), b(5.0f) {}
	Dummy(const Dummy& d) {}
	Dummy(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) : a(a) {}

	Dummy& operator = (const Dummy& d) { return *this; }

	~Dummy()
	{
		printf("Destroyed Dummy\n");
	}

	int a;
	float b;
};

void LoopOverSpan(const crstl::span<Dummy>& dummySpan)
{
#if defined(CRSTL_UNIT_RANGED_FOR)
	for (const Dummy& dummy : dummySpan)
	{
		printf("Dummy: %d\n", dummy.a);
	}
#endif
}

void RunUnitTestsVector()
{
	using namespace crstl_unit;

	int initialVectorSize = 5;

	int so = sizeof(crstl::fixed_vector<Dummy, 32>);

	crstl::fixed_vector<Dummy, 32> crFixedVector(initialVectorSize);
	std::vector<Dummy> stdVector(initialVectorSize);

	crstl_check(crFixedVector.size() == stdVector.size());

	crFixedVector.push_back();
	stdVector.push_back(Dummy());

	crstl_check(crFixedVector.size() == stdVector.size());

	crFixedVector.pop_back();
	stdVector.pop_back();

	crFixedVector.emplace_back(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	stdVector.emplace_back(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	crstl_check(crFixedVector.size() == stdVector.size());

	crFixedVector.clear();
	stdVector.clear();

	crstl_check(crFixedVector.size() == stdVector.size());

	LoopOverSpan(crFixedVector);
}
