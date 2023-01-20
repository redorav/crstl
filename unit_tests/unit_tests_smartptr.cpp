#include "crstl/intrusive_ptr.h"

class MyObject : public crstl::intrusive_ptr_interface_delete
{

};

void RunUnitTestsSmartPtr()
{
	crstl::intrusive_ptr<MyObject> objectHandle = new MyObject();
}