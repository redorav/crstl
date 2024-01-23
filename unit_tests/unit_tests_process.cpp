#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/process.h"
#endif

void RunUnitTestsProcess()
{
	printf("RunUnitTestsProcess\n");

	crstl::process p;
	p = crstl::process("fxc.exe", "/?");

	char buffer[16384];
	p.read_stdout(buffer, 16384);
}