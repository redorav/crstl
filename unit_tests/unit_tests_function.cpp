#include "unit_tests.h"

#include "crstl/fixed_function.h"

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

void FunctionPassByReferenceTest(const crstl::fixed_function<void(int), 8>& myFunction, int k)
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

	crstl::fixed_function<void(), 8> crVoidFunction;
	
	crstl::fixed_function<void(), 16> crVoidFunction2 = []()
	{
		printf("crVoidFunction2\n");
	};

	crstl::fixed_function<void(), 8> crVoidFunction3 = [a]()
	{
		printf("crVoidFunction3 %d\n", a);
	};
	
	crstl::fixed_function<void(), 8> crVoidFunction4 = &DummyVoidFunction;

	crstl::fixed_function<int(int), 8> crIntFunction = &DummyIntFunction;

	crstl::fixed_function<int(int), 8> crIntFunctionNonConst = &DummyIntFunction2;

	const crstl::fixed_function<int(int), 8> crIntFunctionConst = &DummyIntFunction2;

	const crstl::fixed_function<void(void), 8> crMoveOnlyFunction = OperatorStruct();

	crIntFunction = crIntFunctionNonConst;
	crstl::fixed_function<int(int), 8> crIntFunctionCopy1(crIntFunctionNonConst);
	crstl::fixed_function<int(int), 8> crIntFunctionCopy2(crIntFunctionConst);
	crstl::fixed_function<int(int), 8> crIntFunctionMove(std::move(crIntFunctionNonConst));

	crIntFunctionConst(5);

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
	printf("IntFunction %d", i);

	//printf("Hello\n");
	//printf("Hello %d\n", a);
}