#include "crstl/fixed_string.h"
#include <string>

void RunUnitTestsString()
{
	std::string mystds;
	crstl::basic_fixed_string<char, 32> myfs32;
	crstl::basic_fixed_string<char, 32> myfs32_2("String");
	crstl::basic_fixed_string<char, 32> myfs32_foo("foo");
	crstl::basic_fixed_string<char, 32> myfs32_bar("bar");

	crstl::basic_fixed_string<char, 8> myfs8_foo("foo");
	crstl::basic_fixed_string<char, 8> myfs8_bar("bar");

	crstl::basic_fixed_string<wchar_t, 32> mywfs32;

	mystds = "Hello String";
	myfs32 = "Hello String";

	const auto mystdsub = mystds.substr(3, 5);
	const auto myfssub = myfs32.substr(3, 5);

	const char* mystdc_str = mystds.c_str();
	const char* myfsc_str = myfs32.c_str();

	const auto stdfindc = mystds.find('S', 2);
	const auto fsfindc = myfs32.find('S', 2);

	const auto stdfind = mystds.find("ring", 2);
	const auto fsfind = myfs32.find("ring", 2);

	myfs32 = "Hello String";
	myfs32.replace(4, 3, "foo", 3);
	myfs32 = "Hello String";
	myfs32.replace(4, 6, "foo", 3);
	myfs32 = "Hello String";
	myfs32.replace(4, 1, "foo", 3);

	mywfs32 = L"Hello String";
	mywfs32.replace(4, 3, L"foo", 3);
	mywfs32 = L"Hello String";
	mywfs32.replace(4, 6, L"foo", 3);
	mywfs32 = L"Hello String";
	mywfs32.replace(4, 1, L"foo", 3);

	mystds.replace(0, 2, "Hello");

	const auto fsfindfs = myfs32.find(myfs32_2);

	mystds.push_back('a');
	mystds.pop_back();

	// Operator +
	myfs32 = myfs32_foo + myfs32_bar;
	myfs32 = myfs8_foo + myfs8_bar;
	myfs32 = myfs8_foo + myfs32_bar;
}