#include "crstl/unique_ptr.h"
#include "crstl/intrusive_ptr.h"

#include <memory>
#include <stdio.h>
#include <vector>

class MyRefCountObject : public crstl::intrusive_ptr_interface_delete
{
public:

	void PrintName()
	{
		printf("MyRefCountObject\n");
	}

	~MyRefCountObject()
	{
		printf("MyRefCountObject destroyed\n");
	}
};

class MyObject
{
public:

	void PrintName()
	{
		printf("MyObject\n");
	}

	~MyObject()
	{
		printf("MyObject destroyed\n");
	}
};

std::vector<std::unique_ptr<MyObject>> stdUniqueVector;

std::vector<crstl::unique_ptr<MyObject>> crUniqueVector;

void RunUnitTestsSmartPtr()
{
	crstl::intrusive_ptr<MyRefCountObject> objectHandle = new MyRefCountObject();

	// Unique pointer

	std::unique_ptr<MyObject> stdUniquePtr = std::unique_ptr<MyObject>(new MyObject());
	stdUniquePtr->PrintName();
	stdUniquePtr = nullptr;

	crstl::unique_ptr<MyObject> crUniquePtr = crstl::unique_ptr<MyObject>(new MyObject());
	crUniquePtr->PrintName();
	crUniquePtr = nullptr;

	stdUniqueVector.push_back(std::unique_ptr<MyObject>(new MyObject()));
	crUniqueVector.push_back(crstl::unique_ptr<MyObject>(new MyObject()));

	std::unique_ptr<MyObject> stdNullUniquePtr = nullptr;
	stdNullUniquePtr = std::unique_ptr<MyObject>(new MyObject());
	stdNullUniquePtr = nullptr;

	crstl::unique_ptr<MyObject> crNullUniquePtr = nullptr;
	crNullUniquePtr = crstl::unique_ptr<MyObject>(new MyObject());
	crNullUniquePtr = nullptr;
}