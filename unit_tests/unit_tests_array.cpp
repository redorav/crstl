#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/array.h"
#endif

#include <array>

#if defined(CRSTL_FEATURE_CONSTEXPR)

crstl_constexpr crstl::array<int, 3> MyFunction()
{
	return { 1, 2, 3 };
}

crstl_constexpr crstl::array<int, 3> constexprArray = MyFunction();

#endif

void RunUnitTestsArray()
{
	printf("RunUnitTestsArray\n");

	std::array<int, 8> mystdArray = { 1, 2, 3, 4, 5, 6, 7, 8 }; crstl_unused(mystdArray);
	crstl::array<int, 8> crArray32 = { 1, 2, 3, 4, 5, 6, 7, 8 }; crstl_unused(crArray32);

#if defined(CRSTL_UNIT_RANGED_FOR)

	size_t count = 0;
	for (const auto& item : crArray32)
	{
		(void)item;
		count++;
	}

	crstl_assert(crArray32.size() == count);

#endif

#if defined(CRSTL_FEATURE_CONSTEXPR)

	count = 0;
	for (const auto& item : constexprArray)
	{
		(void)item;
		count++;
	}

	crstl_assert(constexprArray.size() == count);

#endif
}