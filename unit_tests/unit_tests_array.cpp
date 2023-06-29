#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/array.h"
#endif

#include <array>

void RunUnitTestsArray()
{
	std::array<int, 8> mystdArray = { 1, 2, 3, 4, 5, 6, 7, 8 }; crstl_unused(mystdArray);
	crstl::array<int, 8> crArray32 = { 1, 2, 3, 4, 5, 6, 7, 8 };

#if defined(CRSTL_UNIT_RANGED_FOR)
	size_t count = 0;
	for (const auto& item : crArray32)
	{
		(void)item;
		count++;
	}

	crstl_assert(crArray32.size() == count);
#endif
}