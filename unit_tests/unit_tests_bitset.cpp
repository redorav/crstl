#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/bitset.h"
#endif

#include <bitset>
#include <stdio.h>

void RunUnitTestsBitset()
{
	using namespace crstl_unit;

	printf("RunUnitTestsBitset\n");

	// Test with 64 bits

	crstl::bitset<64, uint64_t> crBitset64;
	crBitset64[32] = true;
	crstl_check(crBitset64[32] == true);

	crBitset64.set(32, false);
	crstl_check(crBitset64[32] == false);

	crBitset64.flip(32);
	crstl_check(crBitset64[32] == true);

	crBitset64.set();
	crstl_check(crBitset64.all());

	crBitset64.reset();
	crstl_check(crBitset64.none());

	// Test with 256 bits

	crstl::bitset<256, uint64_t> crBitset256;
	crBitset256[32] = true;
	crstl_check(crBitset256[32] == true);

	crBitset256.set(32, false);
	crstl_check(crBitset256[32] == false);

	crBitset256.flip(32);
	crstl_check(crBitset256[32] == true);

	crBitset256.set();
	crstl_check(crBitset256.all());

	crBitset256.reset();
	crstl_check(crBitset256.none());
}