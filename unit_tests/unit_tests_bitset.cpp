#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/bitset.h"
#endif

#include <bitset>

void RunUnitTestsBitset()
{
	printf("RunUnitTestsBitset\n");
	crstl::bitset<64, uint64_t> crBitset; crstl_unused(crBitset);
	std::bitset<64> stdBitset; crstl_unused(stdBitset);
}