#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/fixed_deque.h"
#include "crstl/fixed_vector.h"
#include "crstl/deque.h"
#include "crstl/timer.h"
#endif

#include <deque>

template<int k>
struct static_sizeof;

//crstl_constexpr14 void FixedDequeConstexprFunction()
//{
//	crstl::fixed_deque<int, 32> crFixedDeque;
//
//	//crFixedDeque.push_back();
//	//crFixedDeque.push_back_uninitialized();
//	//crFixedDeque.push_back(Dummy());
//	//static_assert(crFixedDeque.size() == 3, "");
//}

#include <stdlib.h>

void RunUnitTestsDeque()
{
	using namespace crstl_unit;

	Dummy dummy(4, 5.0f);

	begin_test("fixed_deque");
	{
		crstl::fixed_deque<Dummy, 32> crDeque;
		crDeque.resize(16);
		crstl_check(crDeque.size() == 16);

		// We push and pop the same number of elements
		// many times and see that we're doing things
		// correctly
		for (size_t i = 0; i < 16; ++i)
		{
			crDeque.push_back();
			crDeque.pop_front();
			crDeque.emplace_back(1, 2.0f);
			crDeque.pop_front();
			crDeque.emplace_front(2, 3.0f);
			crDeque.pop_back();
			crDeque.push_back();
			crDeque.pop_front();
		}

		crstl_check(crDeque.size() == 16);

		crDeque.resize(16);
		crstl_check(crDeque.size() == 16);

		crstl::fixed_deque<Dummy, 32> crDequeCopy;
		crDequeCopy = crDeque;
		crstl_check(crDequeCopy == crDeque);

		crDeque.resize(10);
		crstl_check(crDeque.size() == 10);

		crDeque.resize(0);
		crstl_check(crDeque.size() == 0);
		crstl_check(crDeque.begin() == crDeque.end());

		for (int i = 0; i < 128; ++i)
		{
			crDeque.push_front();
			crDeque.push_back();
			crDeque.push_front();
			crDeque.push_back();
			crDeque.push_front();
			crDeque.push_back();
			crDeque.pop_front();
			crDeque.pop_back();
			crDeque.pop_back();
			crDeque.pop_front();
			crDeque.pop_back();
			crDeque.pop_front();
		}

		crstl_check(crDeque.size() == 0);
		crstl_check(crDeque.begin() == crDeque.end());

		crDeque.push_front();
		crDeque.push_front(Dummy());
		crDeque.push_front(dummy);
		crDeque.push_front();
		crDeque.push_front();
		crDeque.push_front();

		crDeque.push_back();
		crDeque.push_back(Dummy());
		crDeque.push_back(dummy);
		crDeque.push_back();
		crDeque.push_back();
		crDeque.push_back();

		crDeque[0] = Dummy(42, 42.0f);
		crDeque[10] = Dummy(43, 43.0f);
		crDeque[11] = Dummy(44, 44.0f);

		crDeque.emplace_back(1, 2.0f);
		crDeque.emplace_front(2, 3.0f);

		Dummy& backRef = crDeque.back();
		backRef = Dummy(440, 440.0f);

		Dummy& frontRef = crDeque.front();
		frontRef = Dummy(420, 420.0f);

#if defined(CRSTL_UNIT_RANGED_FOR)
		{
			size_t count = 0;
			for (auto iter = crDeque.begin(); iter != crDeque.end(); ++iter)
			{
				++count;
			}

			crstl_check(crDeque.size() == count);
		}

		{
			size_t count = 0;
			for (const Dummy& d : crDeque)
			{
				(void)d;
				++count;
			}

			crstl_check(crDeque.size() == count);
		}
#endif
	}
	end_test();

	begin_test("deque");
	{
		crstl::deque<Dummy> crDeque(512, Dummy());
		crDeque.clear();

		for (size_t i = 0; i < 200000; ++i)
		{
#if defined(CRSTL_VARIADIC_TEMPLATES)
			int random = rand() % 11;
#else
			int random = rand() % 9;
#endif
			switch(random)
			{
				case 0: crDeque.push_back(); break;
				case 1: crDeque.push_back(Dummy(65, 66.0f)); break;
				case 2: crDeque.push_back(dummy); break;
				case 3: crDeque.push_front(); break;
				case 4: crDeque.push_front(Dummy(65, 66.0f)); break;
				case 5: crDeque.push_front(dummy); break;
				case 7: if (!crDeque.empty()) crDeque.pop_back(); break;
				case 8: if (!crDeque.empty()) crDeque.pop_front(); break;
#if defined(CRSTL_VARIADIC_TEMPLATES)
				case 9: crDeque.emplace_back(67, 68.0f); break;
				case 10: crDeque.emplace_front(67, 68.0f); break;
#endif
			}
		}

		crDeque.push_back();
		crDeque.push_front();
		crDeque.push_back(Dummy(65, 66.0f));
		crDeque.push_front(Dummy(65, 66.0f));
		crDeque.push_back(dummy);
		crDeque.push_front(dummy);

		crDeque[0] = Dummy(150, 162);

		crDeque.back() = Dummy(10, 22);

		crDeque.front() = Dummy(50, 62);

		crDeque.resize(33);

		crstl::deque<Dummy> crDequeResize;
		crDequeResize.resize(40);

		crDequeResize.resize(13);

		crDequeResize.resize_back(512);

		crDequeResize.resize_front(512);

		crDequeResize[0] = Dummy(65, 66.0f);
		crDequeResize.back() = Dummy(65, 66.0f);

#if defined(CRSTL_UNIT_RANGED_FOR)
		{
			size_t count = 0;
			for (auto iter = crDeque.begin(); iter != crDeque.end(); ++iter)
			{
				++count;
			}

			crstl_check(crDeque.size() == count);
		}

		{
			size_t count = 0;
			for (const Dummy& d : crDeque)
			{
				(void)d;
				++count;
			}

			crstl_check(crDeque.size() == count);
		}
#endif

		crDeque.clear();
		crstl_check(crDeque.size() == 0);
		crstl_check(crDeque.begin() == crDeque.end());
	}
	end_test();
}