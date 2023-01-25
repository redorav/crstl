#include "crstl/fixed_vector.h"
#include "crstl/span.h"

#include <vector>

#if crstl_cppversion >= 202002L
#include <span>
#endif

struct Dummy
{
	Dummy() : a(4) {}
	Dummy(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) : a(a) {}

	int a;
};

void LoopOverSpan(const crstl::span<Dummy>& dummySpan)
{
	for (const Dummy& dummy : dummySpan)
	{
		printf("Dummy: %d\n", dummy.a);
	}
}

void RunUnitTestsVector()
{
	crstl::fixed_vector<Dummy, 32> crFixedVector;
	std::vector<int> mystdv;

	crFixedVector.emplace_back(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	crFixedVector.push_back();
	crFixedVector.push_back();
	crFixedVector.push_back();
	crFixedVector.push_back();

	LoopOverSpan(crFixedVector);
}