void RunUnitTestsArray();
void RunUnitTestsBitset();
void RunUnitTestsString();
void RunUnitTestsVector();
void RunUnitTestsFunction();
void RunUnitTestsSmartPtr();

#if defined(__ANDROID__)

#if defined(__cplusplus)
extern "C"
{
#endif

	void android_main(struct android_app* app)

#else

	int main()

#endif
{
	RunUnitTestsArray();
	RunUnitTestsBitset();
	RunUnitTestsString();
	RunUnitTestsVector();
	RunUnitTestsFunction();
	RunUnitTestsSmartPtr();
}

#if defined(__ANDROID__)
}
#endif