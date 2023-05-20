#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/fixed_function.h"
#endif

#include <functional>

#include <stdio.h>

int DummyA = 4;

void DummyVoidFunction()
{
	printf("Hello %d\n", DummyA);
}

int DummyIntFunction(int value)
{
	printf("DummyIntFunction %d\n", value);
	return -1;
}

int DummyIntFunction2(int value)
{
	printf("DummyIntFunction2 %d\n", value);
	return -1;
}

int DummyReturnPassedInt(int value)
{
	return value;
}

void FunctionPassByReferenceTest(const crstl::fixed_function<8, void(int)>& myFunction, int k)
{
	myFunction(k);
}

struct OperatorStruct
{
	OperatorStruct() {}

	void operator()(void) const
	{

	}
};

void RunUnitTestsFunction()
{
	int a = 3;

	int rand1 = rand();
	int rand2 = rand();
	int rand3 = rand();
	int rand4 = rand();
	int rand5 = rand();

	std::function<void()> stdVoidFunction;

	std::function<void()> stdVoidFunction2 = []()
	{
		printf("stdVoidFunction2\n");
	};

	std::function<void()> stdVoidFunction3 = [a]()
	{
		printf("stdVoidFunction3 %d\n", a);
	};

	std::function<void(int)> stdIntFunction;

	stdIntFunction = &DummyIntFunction;

	crstl::fixed_function<8, void()> crVoidFunction;
	
	crstl::fixed_function<16, void()> crVoidFunction2 = []()
	{
		printf("crVoidFunction2\n");
	};

	crstl::fixed_function<8, void()> crVoidFunction3 = [a]()
	{
		printf("crVoidFunction3 %d\n", a);
	};
	
	crstl::fixed_function<8, void()> crVoidFunction4 = &DummyVoidFunction;

	crstl::fixed_function<8, int(int)> crIntFunction = &DummyReturnPassedInt;

	crstl::fixed_function<8, int(int)> crIntFunctionNonConst = &DummyReturnPassedInt;

	crstl::fixed_function<8, int(int)> crIntFunctionMove = &DummyReturnPassedInt;

	const crstl::fixed_function<8, int(int)> crIntFunctionConst = &DummyReturnPassedInt;
	crstl_assert(crIntFunctionConst(rand1) == rand1);

	const crstl::fixed_function<8, void(void)> crMoveOnlyFunction = OperatorStruct();

	crIntFunction = crIntFunctionNonConst;

	crstl::fixed_function<8, int(int)> crIntFunctionCopy1(crIntFunctionNonConst);
	crstl_assert(crIntFunctionCopy1(rand2) == rand2);
	
	crstl::fixed_function<8, int(int)> crIntFunctionCopy2(crIntFunctionConst);
	crstl_assert(crIntFunctionCopy2(rand3) == rand3);

	crstl::fixed_function<8, int(int)> crIntFunctionMoved(std::move(crIntFunctionMove));
	crstl_assert(crIntFunctionMoved(rand4) == rand4);

	crstl::fixed_function<16, int(int)> crIntFunctionCopyDifferentSize(crIntFunctionNonConst);
	crstl_assert(crIntFunctionCopyDifferentSize(rand5) == rand5);

	// Call functions

	if (stdVoidFunction)
	{
		stdVoidFunction();
	}

	if (crVoidFunction)
	{
		crVoidFunction();
	}

	stdVoidFunction2();
	crVoidFunction2();
	
	stdVoidFunction3();
	crVoidFunction3();
	
	//stdVoidFunction4();
	crVoidFunction4();

	int i = crIntFunction(4);
	printf("IntFunction %d\n", i);

	//printf("Hello\n");
	//printf("Hello %d\n", a);
}