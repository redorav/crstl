#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/bitset.h"
#endif

#include <bitset>

void RunUnitTestsBitset()
{
	crstl::bitset<64, uint64_t> mybiset;
	std::bitset<64> stdbitset;
}