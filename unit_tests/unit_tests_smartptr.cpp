#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/unique_ptr.h"
#include "crstl/intrusive_ptr.h"
#endif

#include <vector>
#include <memory>
#include <stdio.h>

std::vector<std::unique_ptr<Dummy>> stdUniqueVector;

std::vector<crstl::unique_ptr<Dummy>> crUniqueVector;

void RunUnitTestsSmartPtr()
{
	printf("RunUnitTestsSmartPtr\n");

	// Unique pointer

	std::unique_ptr<Dummy> stdUniquePtr = std::unique_ptr<Dummy>(new Dummy());
	stdUniquePtr->PrintName();
	stdUniquePtr = nullptr;

	crstl::unique_ptr<Dummy> crUniquePtr = crstl::unique_ptr<Dummy>(new Dummy());
	crUniquePtr->PrintName();
	crUniquePtr = nullptr;

	stdUniqueVector.push_back(std::unique_ptr<Dummy>(new Dummy()));
	crUniqueVector.push_back(crstl::unique_ptr<Dummy>(new Dummy()));

	std::unique_ptr<Dummy> stdNullUniquePtr = nullptr;
	stdNullUniquePtr = std::unique_ptr<Dummy>(new Dummy());
	stdNullUniquePtr = nullptr;

	crstl::unique_ptr<Dummy> crNullUniquePtr = nullptr;
	crNullUniquePtr = crstl::unique_ptr<Dummy>(new Dummy());
	crNullUniquePtr = nullptr;

	if (crUniquePtr) {}
	if (crUniquePtr != nullptr) {}
	if (crUniquePtr != crNullUniquePtr.get()) {}

	crstl::unique_ptr<char[]> crArrayUniquePtr(new char[32]);
	std::unique_ptr<char[]> stdArrayUniquePtr(new char[32]);

	crArrayUniquePtr = nullptr;

	// Intrusive pointer
	crstl::intrusive_ptr<RefCountDummy> objectHandle = new RefCountDummy();

	if (objectHandle)
	{

	}
}