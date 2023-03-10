#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/fixed_string.h"
#include "crstl/string_view.h"
#endif

#include <ctype.h>
#include <string>

#if crstl_cppversion >= 201703L
#include <string_view>
#endif

void RunUnitTestsString()
{
	const char* MyCharString = (const char*)u8"Hello String汉字";
	const wchar_t* MyWCharString = L"Hello String汉字";

	std::string mystds;
	crstl::fixed_string32 myfs32;
	crstl::fixed_string32 myfs32_2("String");
	crstl::fixed_string32 myfs32_foo("foo");
	crstl::fixed_string32 myfs32_bar("bar");

	crstl::fixed_string8 myfs8_foo("foo");
	crstl::fixed_string8 myfs8_bar("bar");

	crstl::fixed_wstring32 mywfs32;

	mystds = "Hello String";
	myfs32 = "Hello String";

	const auto mystdsub = mystds.substr(3, 5);
	const auto myfssub = myfs32.substr(3, 5);

	const char* mystdc_str = mystds.c_str();
	const char* myfsc_str = myfs32.c_str();

	const size_t stdfindc = mystds.find('S', 0);
	const size_t fsfindc = myfs32.find('S', 0);

	const size_t stdfind = mystds.find("rin", 2);
	const size_t fsfind = myfs32.find("rin", 2);

	const size_t stdrfindc = mystds.rfind('S', 6);
	const size_t fsrfindc = myfs32.rfind('S', 6);

	const size_t stdfindr = mystds.rfind("ing");
	const size_t fsfindr = myfs32.rfind("ing");

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

	const char* u1 = (const char*)u8"a";
	const char* u2 = (const char*)u8"\u03EA";
	const char* u3 = (const char*)u8"\u27c1";
	const char* u4 = (const char*)u8"\U00010CFF";

	size_t utf8Offset = 0;
	crstl::codepoint_t cp = crstl::decode_utf8((const uint8_t*)u4, strlen((const char*)u4), utf8Offset);

	myfs32.assign_convert(MyWCharString, crstl::string_length(MyWCharString));
	mywfs32.assign_convert(MyCharString, crstl::string_length(MyCharString));

	// string_view

	crstl::string_view crStringViewEmpty;

	crstl::string_view crStringViewConstChar("String View");

	for (char c : crStringViewConstChar)
	{
		printf("%c ", c);
	}

	printf("\n");

	if (crStringViewConstChar.substr(1, 5) == "tring")
	{
		printf("String was equal\n");
	}

	if (crStringViewConstChar.starts_with('S'))
	{
		printf("Starts with letter S\n");
	}

	if (crStringViewConstChar.ends_with('w'))
	{
		printf("Ends with letter w\n");
	}

	if (crStringViewConstChar.ends_with("View"))
	{
		printf("Ends with View\n");
	}

	myfs32.clear();
	myfs32.append_sprintf("Hello %i", 3);

	char cu = toupper('k');

#if crstl_cppversion >= 201703L
	std::basic_string_view<char> stdStringViewEmpty;

	std::basic_string_view<char> stdStringViewConstChar("String View");
#endif
}