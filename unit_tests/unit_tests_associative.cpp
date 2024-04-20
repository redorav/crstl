#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/fixed_bucket_hashmap.h"
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
template class crstl::fixed_bucket_hashmap<int, int, 64>;
template class crstl::fixed_open_hashmap<int, int, 64>;

template<typename Hashmap>
void RunUnitTestAssociativeT()
{
	using namespace crstl_unit;

	Hashmap crFixedBucketmapInt;

	crstl_check(crFixedBucketmapInt.size() == 0);

	// Iterate through an empty hashmap
	size_t iter_count_empty = 0;
	for (auto iter = crFixedBucketmapInt.begin(), end = crFixedBucketmapInt.end(); iter != end; ++iter)
	{
		iter_count_empty++;
	}
	crstl_check(iter_count_empty == 0);

	// Insert a temporary object
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crFixedBucketmapInt.insert({ ManualKeys[i], Example() });
	}

	crstl_check(crFixedBucketmapInt.size() == crstl::array_size(ManualKeys));

	// Erase all objects manually
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crFixedBucketmapInt.erase(ManualKeys[i]);
	}

	crstl_check(crFixedBucketmapInt.size() == 0);

	// Insert objects created externally
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		Example example = Example();
		crFixedBucketmapInt.insert(ManualKeys[i], example);
	}

	crstl_check(crFixedBucketmapInt.size() == crstl::array_size(ManualKeys));

	// Remove all objects
	crFixedBucketmapInt.clear();

	crstl_check(crFixedBucketmapInt.empty());

	// Emplace objects
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crFixedBucketmapInt.emplace(ManualKeys[i], 1, 2.0f);
	}

	crstl_check(crFixedBucketmapInt.size() == crstl::array_size(ManualKeys));

	// Count iteration count
	size_t iter_count = 0;
	for (auto iter = crFixedBucketmapInt.begin(), end = crFixedBucketmapInt.end(); iter != end; ++iter)
	{
		iter_count++;
	}
	crstl_check(iter_count == crstl::array_size(ManualKeys));

	// Clear via iterators
	for (auto iter = crFixedBucketmapInt.begin(), end = crFixedBucketmapInt.end(); iter != end;)
	{
		iter = crFixedBucketmapInt.erase(iter);
	}

	crstl_check(crFixedBucketmapInt.size() == 0);

	// Emplace or assign objects
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		crFixedBucketmapInt.emplace_or_assign(ManualKeys[i], 1, 2.0f);
	}

	crstl_check(crFixedBucketmapInt.size() == crstl::array_size(ManualKeys));

	// Find the inserted elements
	size_t findElementCount = 0;
	for (size_t i = 0; i < crstl::array_size(ManualKeys); ++i)
	{
		auto iter = crFixedBucketmapInt.find(ManualKeys[i]);
		if (iter != crFixedBucketmapInt.end())
		{
			findElementCount++;
			printf("[Finding] Find %i\n", iter->first);
		}
	}

	crstl_check(crFixedBucketmapInt.size() == findElementCount);

	size_t findElementIter = 0;
	for (auto iter = crFixedBucketmapInt.begin(), end = crFixedBucketmapInt.end(); iter != end; ++iter)
	{
		findElementIter++;
		printf("[Iterators] Key %i\n", iter->first);
	}

	crstl_check(crFixedBucketmapInt.size() == findElementIter);

	size_t findElementRangedForConst = 0;
	for (const auto& iter : crFixedBucketmapInt)
	{
		findElementRangedForConst++;
		printf("[Ranged For Const] Key %i\n", iter.first);
	}

	crstl_check(crFixedBucketmapInt.size() == findElementRangedForConst);

	size_t findElementRangedForNonConst = 0;
	for (auto& iter : crFixedBucketmapInt)
	{
		findElementRangedForNonConst++;
		iter.second = Example(1, 4.0f);
		printf("[Ranged For Non-Const] Key %i\n", iter.first);
	}

	crstl_check(crFixedBucketmapInt.size() == findElementRangedForNonConst);

	// Copy constructor
	Hashmap crFixedBucketmapInt2(crFixedBucketmapInt);
	crstl_check(crFixedBucketmapInt.size() == crFixedBucketmapInt2.size());

	// Iterate over the copied hashmap to check that they were copied correctly
	for (auto iter1 = crFixedBucketmapInt.begin(), iter2 = crFixedBucketmapInt2.begin(); iter1 != crFixedBucketmapInt.end(); ++iter1, ++iter2)
	{
		crstl_check(iter1->second == iter2->second);
	}

	// Copy assignment
	Hashmap crFixedBucketmapInt3;
	crFixedBucketmapInt3 = crFixedBucketmapInt;
	crstl_check(crFixedBucketmapInt.size() == crFixedBucketmapInt3.size());

	// Iterate over the copied hashmap to check that they were copied correctly
	for (auto iter1 = crFixedBucketmapInt.begin(), iter3 = crFixedBucketmapInt3.begin(); iter1 != crFixedBucketmapInt.end(); ++iter1, ++iter3)
	{
		crstl_check(iter1->second == iter3->second);
	}

	crFixedBucketmapInt.clear();

	//crFixedBucketmapInt.debug_validate_empty();

	Hashmap crFixedBucketmapIntInitializerList =
	{
		{ 7, Example() },
		{ 17, Example() },
		{ 57, Example() },
		{ 877, Example() },
		{ 1357, Example() },
		{ 1067, Example() },
	};

	crstl_check(crFixedBucketmapIntInitializerList.size() == 6);

	// Check that insert_or_assign assigns properly
	Hashmap crFixedHashmapInsertAssign;
	crFixedHashmapInsertAssign.insert(7, Example(1, 2.0f));
	crFixedHashmapInsertAssign.insert_or_assign(7, Example(1, 3.0f));
	crstl_check(crFixedHashmapInsertAssign.find(7)->second == Example(1, 3.0f));

	// Check that emplace_or_assign assigns properly
	crFixedHashmapInsertAssign.emplace(18, 3, 6.0f);
	crFixedHashmapInsertAssign.emplace_or_assign(18, Example(4, 25.0f));
	crstl_check(crFixedHashmapInsertAssign.find(18)->second == Example(4, 25.0f));
}

void RunUnitTestsAssociative()
{
	printf("RunUnitTestsAssociative\n");

	crstl::fixed_open_hashmap<int, Example, 64> p;

	RunUnitTestAssociativeT<crstl::open_hashmap<int, Example>>();
	RunUnitTestAssociativeT<crstl::fixed_open_hashmap<int, Example, 64>>();
	RunUnitTestAssociativeT<crstl::fixed_bucket_hashmap<int, Example, 64>>();
}