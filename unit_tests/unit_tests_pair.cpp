#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/pair.h"
#endif

#include <utility>

#include <stdio.h>

void RunUnitTestsPair()
{
	printf("RunUnitTestsPair\n");

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

	DummyA stdFirst = stdPair.first; crstl_unit_unused(stdFirst);
	DummyA crFirst = crPair.first; crstl_unit_unused(crFirst);

	DummyB stdSecond = stdPair.second; crstl_unit_unused(stdSecond);
	DummyB crSecond = crPair.second; crstl_unit_unused(crSecond);

	std::pair<DummyA, DummyB> stdCopyPair(stdPair); crstl_unit_unused(stdCopyPair);
	crstl::pair<DummyA, DummyB> crCopyPair(crPair); crstl_unit_unused(crCopyPair);

	std::pair<DummyA, DummyB> stdAssignPair = stdPair; crstl_unit_unused(stdAssignPair);
	crstl::pair<DummyA, DummyB> crAssignPair = crPair; crstl_unit_unused(crAssignPair);

	DummyC dummyC;

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

	std::pair<DummyA, DummyB> stdInitPair = { 3, 4.0f }; crstl_unit_unused(stdInitPair);
	crstl::pair<DummyA, DummyB> crInitPair = { 3, 4.0f }; crstl_unit_unused(crInitPair);

#endif
}