#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/fixed_deque.h"
#include "crstl/fixed_vector.h"
#endif

#include <deque>

template<int k>
struct static_sizeof;

void RunUnitTestsDeque()
{
	using namespace crstl_unit;

	begin_test("fixed_deque");
	{
		Dummy dummy(4, 5.0f);

		crstl::fixed_deque<Dummy, 32> crDeque;

		crDeque.resize(16);
		crstl_check(crDeque.size() == 16);

		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
		crDeque.emplace_back(1, 2.0f);
		crDeque.pop_front();
		crDeque.emplace_front(2, 3.0f);
		crDeque.pop_front();
		crDeque.push_back();
		crDeque.pop_front();
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
}