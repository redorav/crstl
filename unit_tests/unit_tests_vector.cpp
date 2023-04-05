#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/stack_vector.h"
#include "crstl/fixed_vector.h"
#include "crstl/vector.h"
#include "crstl/span.h"
#endif

#include <vector>
#include <stdio.h>

#if crstl_cppversion >= 202002L
	#include <span>
#endif

void LoopOverSpan(const crstl::span<Dummy>& dummySpan)
{
	(void)dummySpan;
#if defined(CRSTL_UNIT_RANGED_FOR)
	for (const Dummy& dummy : dummySpan)
	{
		printf("Dummy: %d\n", dummy.a);
	}
#endif
}

void RunUnitTestsVector()
{
	using namespace crstl_unit;

	Dummy dummyArray[] = { Dummy(1, 2.0f), Dummy(3, 4.0f), Dummy(5, 6.0f), Dummy(7, 8.0f) };
	crstl::intrusive_ptr<Dummy> intrusiveArray[] = { crstl::intrusive_ptr<Dummy>(), crstl::intrusive_ptr<Dummy>(), crstl::intrusive_ptr<Dummy>(), crstl::intrusive_ptr<Dummy>() };

	begin_test("fixed_vector");
	{
		int initialVectorSize = 5;

		crstl::fixed_vector<Dummy, 32> crFixedVector(initialVectorSize);
		crstl::fixed_vector<Dummy, 32> crFixedVector2(initialVectorSize);
		std::vector<Dummy> stdVector(initialVectorSize);

		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.push_back();
		stdVector.push_back(Dummy());

		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.pop_back();
		stdVector.pop_back();

		crFixedVector.emplace_back(0, 1.0f, 2, 3, 4, 5, 6, 7, 8, 9);
#if defined(CRSTL_VARIADIC_TEMPLATES)
		stdVector.emplace_back(0, 1.0f, 2, 3, 4, 5, 6, 7, 8, 9);
#else
		stdVector.push_back(Dummy(0, 1.0f, 2, 3, 4, 5, 6, 7, 8, 9));
#endif
		crstl_check(crFixedVector.size() == stdVector.size());

		// Iterators don't exist before C++11
		crFixedVector = crstl::fixed_vector<Dummy, 32>(&dummyArray[0], &dummyArray[3]);
		//stdVector = std::vector<Dummy>(&dummyArray[0], &dummyArray[3]);
		//crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.clear();
		stdVector.clear();
		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.resize(5);
		stdVector.resize(5);
		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.clear();
		stdVector.clear();

		crFixedVector.resize(5, Dummy(40, 50.0f, nullptr));
		stdVector.resize(5, Dummy(40, 50.0f, nullptr));
		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.push_back(Dummy());
		stdVector.push_back(Dummy());
		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.emplace_back(Dummy());
		stdVector.emplace_back(Dummy());
		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.clear();
		crFixedVector.push_back(crstl::move(Dummy()));

		LoopOverSpan(crFixedVector2);

		crstl::fixed_vector<crstl::intrusive_ptr<Dummy>, 32> crFixedVectorIntrusive(&intrusiveArray[0], &intrusiveArray[0] + 3);
		crstl::fixed_vector<crstl::intrusive_ptr<Dummy>, 32> crFixedVectorIntrusive2 = crFixedVectorIntrusive;

		//std::vector<crstl::intrusive_ptr<Dummy>> stdIntrusiveVector(&intrusiveArray[0], &intrusiveArray[3]);
		//std::vector<crstl::intrusive_ptr<Dummy>> stdIntrusiveVector2 = stdIntrusiveVector;

		crstl::fixed_vector<Dummy*, 32> crFixedVectorPointer;
		crFixedVectorPointer.push_back(new DummyChild());

#if defined(CRSTL_UNIT_UNIFORM_INITIALIZATION)

		crFixedVector = { Dummy(), Dummy(), Dummy() };
		stdVector = { Dummy(), Dummy(), Dummy() };
		crstl_check(crFixedVector.size() == stdVector.size());

		// Test the move version
		crstl::fixed_vector<Dummy, 32> crFixedVectorBraced = { Dummy() };
		crstl::fixed_vector<Dummy, 32> crFixedVectorBraced2 = { Dummy(), Dummy() };
		crstl::fixed_vector<Dummy, 32> crFixedVectorBraced3 = { Dummy(1, 2.0f), Dummy(3, 4.0f), Dummy(5, 6.0f) };
		crstl_check(crFixedVectorBraced.size() == 1);
		crstl_check(crFixedVectorBraced2.size() == 2);
		crstl_check(crFixedVectorBraced3.size() == 3);
		
		crFixedVectorBraced = { Dummy() };
		crFixedVectorBraced2 = { Dummy(), Dummy() };
		crFixedVectorBraced3 = { Dummy(1, 2.0f), Dummy(3, 4.0f), Dummy(5, 6.0f) };
		crstl_check(crFixedVectorBraced.size() == 1);
		crstl_check(crFixedVectorBraced2.size() == 2);
		crstl_check(crFixedVectorBraced3.size() == 3);
		
		// Test the const T& version
		Dummy dummy;
		crFixedVectorBraced = { dummy };
		crFixedVectorBraced2 = { dummy, dummy };
		crFixedVectorBraced3 = { dummy, dummy, dummy };
		crstl_check(crFixedVectorBraced.size() == 1);
		crstl_check(crFixedVectorBraced2.size() == 2);
		crstl_check(crFixedVectorBraced3.size() == 3);

#endif
	}
	end_test();

	begin_test("vector");
	{
		int initialVectorSize = 5;

		crstl::vector<Dummy> crVector(initialVectorSize);
		std::vector<Dummy> stdVector(initialVectorSize);

		crstl_check(crVector.size() == stdVector.size());

		crVector.clear();
		stdVector.clear();

		crstl_check(crVector.size() == stdVector.size());

		crVector.push_back();
		stdVector.push_back(Dummy());

		crstl_check(crVector.size() == stdVector.size());

		crVector.resize(6, Dummy(20, 70.0f, nullptr));
		stdVector.resize(6, Dummy(20, 70.0f, nullptr));

		crstl_check(crVector.size() == stdVector.size());

		crVector.resize(3);
		stdVector.resize(3);

		crstl_check(crVector.size() == stdVector.size());

		{
			size_t count = 0;
			for (auto iter = crVector.begin(); iter != crVector.end(); ++iter)
			{
				count++;
			}
			crstl_check(crVector.size() == count);
		}

#if defined(CRSTL_UNIT_RANGED_FOR)
		{
			size_t count = 0;
			for (const auto& dummy : crVector)
			{
				count++;
			}

			crstl_check(crVector.size() == count);
		}
#endif
	}
	end_test();

	begin_test("stack_vector");
	{
		crstl::stack_vector<Dummy> crStackVector(crstl_alloca_t(Dummy, 32));
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());
		crStackVector.push_back(Dummy());

		crStackVector.emplace_back();

		crStackVector[4].b = 42;
	}
	end_test();
}
