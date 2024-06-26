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

#if CRSTL_CPPVERSION >= CRSTL_CPP20
#include <span>
#endif

void RunUnitTestsVectorNoStd()
{
	using namespace crstl_unit;

	begin_test("fixed_vector");
	{
		size_t initialVectorSize = 5;
		crstl::fixed_vector<Dummy, 32> crFixedVector(initialVectorSize);
		crFixedVector.push_back();
		crFixedVector.push_back_uninitialized();
	}
	end_test();
}

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

//crstl_constexpr void VectorConstexprFunction()
//{
//	crstl::fixed_vector<int, 32> crConstexprFixedVector;
//	crConstexprFixedVector.push_back();
//#if defined(CRSTL_FEATURE_VARIADIC_TEMPLATES)
//	crConstexprFixedVector.emplace_back();
//#endif
//}

// Explicit instantiation to help catch errors
template class crstl::vector<int, crstl::allocator>;
template class crstl::fixed_vector<int, 32>;
template class crstl::stack_vector<int>;

void RunUnitTestsVectorStdCompare()
{
	using namespace crstl_unit;

	Dummy dummyArray[] = { Dummy(1, 2.0f), Dummy(3, 4.0f), Dummy(5, 6.0f), Dummy(7, 8.0f) };
	crstl::intrusive_ptr<Dummy> intrusiveArray[] = { crstl::intrusive_ptr<Dummy>(), crstl::intrusive_ptr<Dummy>(), crstl::intrusive_ptr<Dummy>(), crstl::intrusive_ptr<Dummy>() };

	begin_test("fixed_vector");
	{
		size_t initialVectorSize = 5;

		crstl::fixed_vector<Dummy, 32> crFixedVector(initialVectorSize);
		crstl::fixed_vector<Dummy, 32> crFixedVector2(initialVectorSize);
		std::vector<Dummy> stdVector(initialVectorSize);

		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.push_back();
		stdVector.push_back(Dummy());

		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.pop_back();
		stdVector.pop_back();

#if defined(CRSTL_FEATURE_VARIADIC_TEMPLATES)
		crFixedVector.emplace_back(0, 1.0f, 2, 3, 4, 5, 6, 7, 8, 9);
		stdVector.emplace_back(0, 1.0f, 2, 3, 4, 5, 6, 7, 8, 9);
#else
		crFixedVector.push_back(Dummy(0, 1.0f, 2, 3, 4, 5, 6, 7, 8, 9));
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

		crFixedVector.push_back_uninitialized();
		stdVector.push_back(Dummy());
		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.clear();
		stdVector.clear();

		crFixedVector.resize(5, Dummy(40, 50.0f, nullptr));
		stdVector.resize(5, Dummy(40, 50.0f, nullptr));
		crstl_check(crFixedVector.size() == stdVector.size());

		crFixedVector.push_back(Dummy());
		stdVector.push_back(Dummy());
		crstl_check(crFixedVector.size() == stdVector.size());

#if defined(CRSTL_FEATURE_VARIADIC_TEMPLATES)
		crFixedVector.emplace_back(Dummy());
		stdVector.emplace_back(Dummy());
		crstl_check(crFixedVector.size() == stdVector.size());
#endif

		crFixedVector = crFixedVector2;
		crstl_check(crFixedVector == crFixedVector2);

		crFixedVector.clear();
		crFixedVector.push_back(crstl::move(Dummy()));

		LoopOverSpan(crFixedVector2);

		crstl::fixed_vector<crstl::intrusive_ptr<Dummy>, 32> crFixedVectorIntrusive(&intrusiveArray[0], &intrusiveArray[0] + 3);
		crstl::fixed_vector<crstl::intrusive_ptr<Dummy>, 32> crFixedVectorIntrusive2 = crFixedVectorIntrusive;

		//std::vector<crstl::intrusive_ptr<Dummy>> stdIntrusiveVector(&intrusiveArray[0], &intrusiveArray[3]);
		//std::vector<crstl::intrusive_ptr<Dummy>> stdIntrusiveVector2 = stdIntrusiveVector;

		crstl::fixed_vector<Dummy*, 32> crFixedVectorPointer;
		crFixedVectorPointer.push_back(new DummyChild());

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

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

		crstl::fixed_vector<int, 32> crFixedVectorTrivial(initialVectorSize, 10);

		crstl::fixed_vector<int, 32> crFixedVectorTrivial2 = crFixedVectorTrivial;
#endif
	}
	end_test();

	begin_test("vector");
	{
		crstl::vector<Dummy> crEmptyVector;
		std::vector<Dummy> stdEmptyVector;

		crstl_check(crEmptyVector.size() == stdEmptyVector.size());

		crEmptyVector.push_back();
		stdEmptyVector.push_back(Dummy());

		crstl_check(crEmptyVector.size() == stdEmptyVector.size());

		size_t initialVectorSize = 5;

		crstl::vector<Dummy> crVector(initialVectorSize);
		std::vector<Dummy> stdVector(initialVectorSize);

		crstl_check(crVector.size() == stdVector.size());

		crVector.clear();
		stdVector.clear();

		crstl_check(crVector.size() == stdVector.size());

		crVector.push_back();
		stdVector.push_back(Dummy());

		crstl_check(crVector.size() == stdVector.size());

		crVector.push_back_uninitialized();
		stdVector.push_back(Dummy());

		crstl_check(crVector.size() == stdVector.size());

#if defined(CRSTL_FEATURE_VARIADIC_TEMPLATES)
		crVector.emplace_back(9, 20.0f);
		stdVector.emplace_back(9, 20.0f);
#else
		stdVector.push_back(Dummy(9, 20.0f));
#endif

		crstl_check(crVector.size() == stdVector.size());

		crVector.resize(6, Dummy(20, 70.0f, nullptr));
		stdVector.resize(6, Dummy(20, 70.0f, nullptr));

		crstl_check(crVector.size() == stdVector.size());

		crVector.resize(3);
		stdVector.resize(3);

		crstl_check(crVector.size() == stdVector.size());

		crVector.shrink_to_fit();

		crstl_check(crVector.size() <= crVector.capacity());

		// Vector reservation tests
		{
			Dummy dummyCompare(20, 70.0f, nullptr);
			crstl::vector<Dummy> crResizeVector;
			crResizeVector.resize(10, dummyCompare);
			for (uint32_t i = 0; i < crResizeVector.size(); ++i)
			{
				crstl_check(crResizeVector[i] == dummyCompare);
			}
		}

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
			for (const auto& _ : crVector)
			{
				(void)_; count++;
			}

			crstl_check(crVector.size() == count);
		}
#endif

		crstl::vector<crstl::intrusive_ptr<Dummy>> crVectorResize;

		// Check that we are destroying things properly when resizing
		for (size_t i = 0; i < 32; ++i)
		{
			crVectorResize.push_back(crstl::intrusive_ptr<Dummy>(new Dummy()));
		}

		for (size_t i = 0; i < crVectorResize.size(); ++i)
		{
			crstl_check(crVectorResize[i]->get_ref() == 1);
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)
		crstl::vector<Dummy> crVectorInitializerList =
		{
			Dummy(1, 2.0f),
			Dummy(5, 67.0f),
			Dummy(3, 98.0f),
			Dummy(0, 13.0f),
			Dummy(54, 90.0f)
		};

		crstl_check(crVectorInitializerList.size() == 5);
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

#if defined(CRSTL_FEATURE_VARIADIC_TEMPLATES)
		crStackVector.emplace_back();
#endif

		crStackVector[4].b = 42;
	}
	end_test();
}

void RunUnitTestsVector()
{
	printf("RunUnitTestsVector\n");

	RunUnitTestsVectorNoStd();
	RunUnitTestsVectorStdCompare();
}
