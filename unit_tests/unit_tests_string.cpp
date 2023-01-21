#include "crstl/fixed_string.h"
#include <string>

void RunUnitTestsString()
{
	const char* MyCharString = u8"Hello String汉字";
	const wchar_t* MyWCharString = L"Hello String汉字";

	std::string mystds;
	crstl::fixed_string<char, 32> myfs32;
	crstl::fixed_string<char, 32> myfs32_2("String");
	crstl::fixed_string<char, 32> myfs32_foo("foo");
	crstl::fixed_string<char, 32> myfs32_bar("bar");

	crstl::fixed_string<char, 8> myfs8_foo("foo");
	crstl::fixed_string<char, 8> myfs8_bar("bar");

	crstl::fixed_string<wchar_t, 32> mywfs32;

	mystds = "Hello String";
	myfs32 = "Hello String";

	const auto mystdsub = mystds.substr(3, 5);
	const auto myfssub = myfs32.substr(3, 5);

	const char* mystdc_str = mystds.c_str();
	const char* myfsc_str = myfs32.c_str();

	const auto stdfindc = mystds.find('S', 0);
	const auto fsfindc = myfs32.find('S', 0);

	const auto stdfind = mystds.find("rin", 2);
	const auto fsfind = myfs32.find("rin", 2);

	const auto stdfindcr = mystds.rfind('S', 6);
	const auto fsfindcr = myfs32.rfind('S', 6);

	const auto stdfindr = mystds.rfind("ing");
	const auto fsfindr = myfs32.rfind("ing");

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

	const char* u1 = u8"a";
	const char* u2 = u8"\u03EA";
	const char* u3 = u8"\u27c1";
	const char* u4 = u8"\U00010CFF";

	size_t utf8Offset = 0;
	crstl::codepoint_t cp = crstl::decode_utf8((const uint8_t*)u4, strlen((const char*)u4), utf8Offset);

	myfs32.append_convert(MyWCharString, crstl::string_length(MyWCharString));

	mywfs32.append_convert(MyCharString, crstl::string_length(MyCharString));
}