#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/bitset.h"
#endif

#include <bitset>

void RunUnitTestsBitset()
{
	crstl::bitset<64, crstl::uint64_t> crBitset; crstl_unused(crBitset);
	std::bitset<64> stdBitset; crstl_unused(stdBitset);
}