#pragma once

// We use this to disable some unit tests on old compilers just so they compile
// and run the rest of the tests. Put here so we don't accidentally use it in 
// the actual library
#if !(defined(_MSC_VER) && _MSC_VER < 1900)
#define CRSTL_UNIT_RANGED_FOR
#define CRSTL_UNIT_UNICODE_LITERALS
#endif

namespace crstl_unit
{
	void begin_test(const char* name);

	void end_test();

	#define crstl_check(x) \
		begin_test_case(#x, __LINE__, __FILE__); \
		check_condition(x); \
		end_test_case();
	
	void begin_test_case(const char* functionString, int lineNumber, const char* file);

	void check_condition(bool condition);

	void end_test_case();
};