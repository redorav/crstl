#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/atomic.h"
#endif

#include <atomic>

void RunUnitTestsAtomic()
{
	using namespace crstl_unit;

	crstl::atomic<uint32_t> crAtomicU32;
	crAtomicU32++;
	crstl_check(crAtomicU32 == 1);

	crstl::atomic<int32_t> crAtomicI32;
	crAtomicI32++;
	crstl_check(crAtomicI32 == 1);

	crstl::atomic<char8_t> crAtomicChar8;
	crAtomicChar8++;
}