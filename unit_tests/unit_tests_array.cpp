#include "unit_tests.h"

#include "crstl/array.h"
#include <array>

void RunUnitTestsArray()
{
	std::array<int, 8> mystdArray = { 1, 2, 3, 4, 5, 6, 7, 8 };
	crstl::array<int, 8> crArray32 = { 1, 2, 3, 4, 5, 6, 7, 8 };

#if defined(CRSTL_UNIT_RANGED_FOR)
	for (const auto& item : crArray32)
	{

	}
#endif
}