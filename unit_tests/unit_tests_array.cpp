#include "crstl/array.h"
#include <array>

void RunUnitTestsArray()
{
	std::array<int, 8> mystdArray = { 1, 2, 3, 4, 5, 6, 7, 8 };
	crstl::array<int, 8> myArray32 = { 1, 2, 3, 4, 5, 6, 7, 8 };

	for (const auto& item : myArray32)
	{

	}
}