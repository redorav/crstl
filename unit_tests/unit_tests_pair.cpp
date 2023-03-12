#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/pair.h"
#endif

#include <utility>

void RunUnitTestsPair()
{
	struct DummyC
	{
		DummyC() : i(0), f(0.0f) {}

		int i;
		float f;
	};

	struct DummyA
	{
		DummyA() { i = 0; }
		DummyA(int k) : i(k) {}
		DummyA(DummyC dc) : i(dc.i) {}

		int i;
	};

	struct DummyB
	{
		DummyB() { f = 0; }
		DummyB(float k) : f(k) {}
		DummyB(DummyC dc) { f = dc.f; }

		float f;
	};

	std::pair<DummyA, DummyB> stdPair;
	crstl::pair<DummyA, DummyB> crPair;

	DummyA stdFirst = stdPair.first;
	DummyA crFirst = crPair.first;

	DummyB stdSecond = stdPair.second;
	DummyB crSecond = crPair.second;

	std::pair<DummyA, DummyB> stdCopyPair(stdPair);
	crstl::pair<DummyA, DummyB> crCopyPair(crPair);

	std::pair<DummyA, DummyB> stdAssignPair = stdPair;
	crstl::pair<DummyA, DummyB> crAssignPair = crPair;

	DummyC dummyC;

	std::pair<DummyA, DummyB> stdInitPair = { 3, 4.0f };
	crstl::pair<DummyA, DummyB> crInitPair(dummyC, dummyC);
}