#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/fixed_open_hashmap.h"
#include "crstl/open_hashmap.h"
#include "crstl/timer.h"
#include "crstl/type_array.h"
#endif

#include <functional>
#include <unordered_map>
#include <string>
#include <stdio.h>

namespace crstl
{
	template<typename T> struct hash;
	template<> struct hash<std::string> { size_t operator()(const std::string& value) { std::hash<std::string> hasher; return hasher(value); } };
};

int ManualKeys[] = { 16, 48, 80, 76, 86, 96, 106, 116, 126, 136, 146, 156, 166, 176, 186, 196, 213, 236, 254, 268, 272 };

// Explicit instantiation to help catch errors
template class crstl::fixed_open_hashmap<int, int, 64>;

template<typename Hashmap>
void RunUnitTestHashmapT()
{
	using namespace crstl_unit;

	Hashmap crHashmap;

	crstl_check(crHashmap.size() == 0);

	// Iterate through an empty hashmap
	size_t iter_count_empty = 0;
	for (auto iter = crHashmap.begin(), end = crHashmap.end(); iter != end; ++iter)
	{
		iter_count_empty++;
	}
	crstl_check(iter_count_empty == 0);

	// Insert a temporary object
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crHashmap.insert({ ManualKeys[i], Example() });
	}

	crstl_check(crHashmap.size() == crstl::array_size(ManualKeys));

	// Erase all objects manually
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crHashmap.erase(ManualKeys[i]);
	}

	crstl_check(crHashmap.size() == 0);

	// Insert objects created externally
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		Example example = Example();
		crHashmap.insert(ManualKeys[i], example);
	}

	crstl_check(crHashmap.size() == crstl::array_size(ManualKeys));

	// Remove all objects
	crHashmap.clear();

	crstl_check(crHashmap.empty());

	// Emplace objects
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crHashmap.emplace(ManualKeys[i], 1, 2.0f);
	}

	crstl_check(crHashmap.size() == crstl::array_size(ManualKeys));

	// Count iteration count
	size_t iter_count = 0;
	for (auto iter = crHashmap.begin(), end = crHashmap.end(); iter != end; ++iter)
	{
		iter_count++;
	}
	crstl_check(iter_count == crstl::array_size(ManualKeys));

	// Clear via iterators
	for (auto iter = crHashmap.begin(), end = crHashmap.end(); iter != end;)
	{
		iter = crHashmap.erase(iter);
	}

	crstl_check(crHashmap.size() == 0);

	// Emplace or assign objects
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crHashmap.emplace_or_assign(ManualKeys[i], 1, 2.0f);
	}

	crstl_check(crHashmap.size() == crstl::array_size(ManualKeys));

	// Find the inserted elements
	size_t findElementCount = 0;
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		auto iter = crHashmap.find(ManualKeys[i]);
		if (iter != crHashmap.end())
		{
			findElementCount++;
			//printf("[Finding] Find %i\n", iter->first);
		}
	}

	crstl_check(crHashmap.size() == findElementCount);

	size_t findElementIter = 0;
	for (auto iter = crHashmap.begin(), end = crHashmap.end(); iter != end; ++iter)
	{
		findElementIter++;
		//printf("[Iterators] Key %i\n", iter->first);
	}

	crstl_check(crHashmap.size() == findElementIter);

	size_t findElementRangedForConst = 0;
	for (const auto& iter : crHashmap)
	{
		crstl_unused(iter);
		findElementRangedForConst++;
		//printf("[Ranged For Const] Key %i\n", iter.first);
	}

	crstl_check(crHashmap.size() == findElementRangedForConst);

	size_t findElementRangedForNonConst = 0;
	for (auto& iter : crHashmap)
	{
		findElementRangedForNonConst++;
		iter.second = Example(1, 4.0f);
		//printf("[Ranged For Non-Const] Key %i\n", iter.first);
	}

	crstl_check(crHashmap.size() == findElementRangedForNonConst);

	// Copy constructor
	Hashmap crHashmapInt2(crHashmap);
	crstl_check(crHashmap.size() == crHashmapInt2.size());

	// Iterate over the copied hashmap to check that they were copied correctly
	for (auto iter1 = crHashmap.begin(), iter2 = crHashmapInt2.begin(); iter1 != crHashmap.end(); ++iter1, ++iter2)
	{
		crstl_check(iter1->second == iter2->second);
	}

	// Copy assignment
	Hashmap crHashmapInt3;
	crHashmapInt3 = crHashmap;
	crstl_check(crHashmap.size() == crHashmapInt3.size());

	// Iterate over the copied hashmap to check that they were copied correctly
	for (auto iter1 = crHashmap.begin(), iter3 = crHashmapInt3.begin(); iter1 != crHashmap.end(); ++iter1, ++iter3)
	{
		crstl_check(iter1->second == iter3->second);
	}

	crHashmap.clear();

	Hashmap crHashmapIntInitializerList =
	{
		{ 7, Example() },
		{ 17, Example() },
		{ 57, Example() },
		{ 877, Example() },
		{ 1357, Example() },
		{ 1067, Example() },
	};

	crstl_check(crHashmapIntInitializerList.size() == 6);

	// Check that insert_or_assign assigns properly
	Hashmap crHashmapInsertAssign;
	crHashmapInsertAssign.insert(7, Example(1, 2.0f));
	crHashmapInsertAssign.insert_or_assign(7, Example(1, 3.0f));
	crstl_check(crHashmapInsertAssign.find(7)->second == Example(1, 3.0f));

	// Check that emplace_or_assign assigns properly
	crHashmapInsertAssign.emplace(18, 3, 6.0f);
	crHashmapInsertAssign.emplace_or_assign(18, Example(4, 25.0f));
	crstl_check(crHashmapInsertAssign.find(18)->second == Example(4, 25.0f));
}
}

void RunUnitTestsAssociative()
{
	printf("RunUnitTestsAssociative\n");

	RunUnitTestAssociativeT<crstl::open_hashmap<int, Example>>();
	RunUnitTestAssociativeT<crstl::fixed_open_hashmap<int, Example, 64>>();
}