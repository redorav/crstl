#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/string.h"
#include "crstl/fixed_string.h"
#include "crstl/string_view.h"
#include "crstl/move_forward.h"
#endif

#include <ctype.h>
#include <string>

#if crstl_cppversion >= 201703L
#include <string_view>
#endif

//constexpr void ConstexprFunction()
//{
//	constexpr crstl::string crConstexprString = "Hello";
//}

void RunUnitTestsString()
{
	using namespace crstl_unit;

	// Utility Strings

	const char* HelloString = "Hello String";
	const wchar_t* WHelloString = L"Hello String";
	const char* Hello = "Hello";
	const char* String = "String";
	
	char HelloStringArray[64] = {};
	sprintf(HelloStringArray, HelloString);

	char HelloArray[32] = {};
	sprintf(HelloArray, Hello);

	char StringArray[32] = {};
	sprintf(StringArray, String);

	// Unicode Strings

	// Taberu
	const char* TaberuUtf8 = (const char*)u8"\u98df\u98df\u98df\u98df"; // Correct
	const wchar_t* TaberuWchar = L"\u98df\u98df\u98df\u98df\u98df\u98df\u98df\u98df"; // Correct
	const char8_t* TaberuChar8 = (const char8_t*)u8"\u98df\u98df\u98df\u98df"; // Correct
	const char16_t* TaberuChar16 = u"\u98df\u98df\u98df\u98df"; // Correct
	const char32_t* TaberuChar32 = U"\u98df\u98df\u98df\u98df"; // Correct

	size_t lengthTaberuUtf8 = strlen((const char*)TaberuUtf8); // Correct
	size_t lengthTaberuWchar = wcslen(TaberuWchar); // Correct
	size_t lengthTaberuChar8 = strlen((const char*)TaberuChar8); // Correct
	//size_t lengthTaberuChar16 = wcslen(TaberuChar16); // Correct
	//size_t lengthTaberuChar32 = wcslen(TaberuChar32); // Correct

	// 食
	const char* TaberuUtf8_lit = (const char*)"食食食食";
	const wchar_t* TaberuWchar_lit = L"食食食食";
	const char8_t* TaberuChar8_lit = (const char8_t*)u8"食食食食";
	const char16_t* TaberuChar16_lit = u"食食食食";
	const char32_t* TaberuChar32_lit = U"食食食食";

	size_t lengthTaberuUtf8_lit = strlen((const char*)TaberuUtf8_lit); // Correct
	size_t lengthTaberuWchar_lit = wcslen(TaberuWchar_lit); // Incorrect
	size_t lengthTaberuChar8_lit = strlen((const char*)TaberuChar8_lit); // Incorrect
	//size_t lengthTaberuChar16_lit = wcslen(TaberuChar16_lit);
	//size_t lengthTaberuChar32_lit = wcslen(TaberuChar32_lit);

	begin_test("fixed_string");
	{
		// Constructors

		std::string mystds;
		crstl::fixed_string32 crFixedString32;
		crstl::fixed_string32 crFixedString32_2("String");
		crstl::fixed_string32 crFixedString32_3(HelloString);
		crstl::fixed_string32 crFixedString32_4(crFixedString32_2.c_str());
		crstl::fixed_string32 crFixedString32_foo("foo");
		crstl::fixed_string32 crFixedString32_bar("bar");

		crstl::fixed_string8 myfs8_foo("foo");
		crstl::fixed_string8 myfs8_bar("bar");

		crstl::fixed_wstring32 mywfs32;

		// Assignment

		mystds = "Hello String";
		mystds = mystds.c_str();
		crFixedString32 = "Hello String";
		crFixedString32 = HelloString;
		crFixedString32 = crFixedString32;
		crFixedString32 = crFixedString32.c_str();

		const auto myfssub = crFixedString32.substr(3, 5);
		const auto mystdsub = mystds.substr(3, 5);
		//crstl_check(myfssub == mystdsub);

		const char* myfsc_str = crFixedString32.c_str();
		const char* mystdc_str = mystds.c_str();

		const size_t fsfindc = crFixedString32.find('S', 0);
		const size_t stdfindc = mystds.find('S', 0);
		crstl_check(fsfindc == stdfindc);

		const size_t fsfind = crFixedString32.find("rin", 2);
		const size_t stdfind = mystds.find("rin", 2);
		crstl_check(fsfind == stdfind);

		const size_t fsrfindc = crFixedString32.rfind('S', 6);
		const size_t stdrfindc = mystds.rfind('S', 6);
		crstl_check(fsrfindc == stdrfindc);

		const size_t fsfindr = crFixedString32.rfind("ing");
		const size_t stdfindr = mystds.rfind("ing");
		crstl_check(fsfindr == stdfindr);

		crFixedString32 = "Hello String";
		crFixedString32.replace(4, 3, "foo", 3);
		crFixedString32 = "Hello String";
		crFixedString32.replace(4, 6, "foo", 3);
		crFixedString32 = "Hello String";
		crFixedString32.replace(4, 1, "foo", 3);

		mywfs32 = L"Hello String";
		mywfs32.replace(4, 3, L"foo", 3);
		mywfs32 = L"Hello String";
		mywfs32.replace(4, 6, L"foo", 3);
		mywfs32 = L"Hello String";
		mywfs32.replace(4, 1, L"foo", 3);

		mystds.replace(0, 2, "Hello");

		const auto fsfindfs = crFixedString32.find(crFixedString32_2);

		mystds.push_back('a');
		mystds.pop_back();

		// Operator +
		crFixedString32 = crFixedString32_foo + crFixedString32_bar;
		crFixedString32 = myfs8_foo + myfs8_bar;
		crFixedString32 = myfs8_foo + crFixedString32_bar;

		crFixedString32.assign_convert(WHelloString, crstl::string_length(WHelloString));
		mywfs32.assign_convert(HelloString, crstl::string_length(HelloString));

		// erase
		{
			crstl::fixed_string32 crFixedStringErase = "Hello String";
			std::string stdStringErase = "Hello String";

			crFixedStringErase.erase(2, 5);
			stdStringErase.erase(2, 5);
		}

		// operators
		{
			crstl::fixed_string32 crStringCompare = "Hello World";
			std::string stdStringCompare = "Hello World";

			bool crLessThanHello = crStringCompare < "Hello";
			bool stdLessThanHello = stdStringCompare < "Hello";
			crstl_check(crLessThanHello == stdLessThanHello);

			bool crGreaterThanHello = crStringCompare > "Hello";
			bool stdGreaterThanHello = stdStringCompare > "Hello";
			crstl_check(crGreaterThanHello == stdGreaterThanHello);

			bool crEqualHello = crStringCompare == "Hello World";
			bool stdEqualHello = stdStringCompare == "Hello World";
			crstl_check(crEqualHello == stdEqualHello);

			bool crLessThanHelloWordl = crStringCompare < "Hello Wordl";
			bool stdLessThanHelloWordl = stdStringCompare < "Hello Wordl";
			crstl_check(crLessThanHelloWordl == stdLessThanHelloWordl);

			bool crLessThanHelloWordl2 = crStringCompare < "Hello Wordl2";
			bool stdLessThanHelloWordl2 = stdStringCompare < "Hello Wordl2";
			crstl_check(crLessThanHelloWordl2 == stdLessThanHelloWordl2);

			int crCompareHelloWordl2 = crStringCompare.compare("Hello Wordl2");
			int stdCompareHelloWordl2 = stdStringCompare.compare("Hello Wordl2");
			crstl_check((crCompareHelloWordl2 > 0) == (stdCompareHelloWordl2 > 0));
		}

		// string_view

		crstl::string_view crStringViewEmpty;

		crstl::string_view crStringViewConstChar("String View");

#if defined(CRSTL_UNIT_RANGED_FOR)
		for (char c : crStringViewConstChar)
		{
			printf("%c ", c);
		}

#endif

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

		crFixedString32.clear();
		crFixedString32.append_sprintf("Hello %i", 3);

#if crstl_cppversion >= 201703L
		std::basic_string_view<char> stdStringViewEmpty;

		std::basic_string_view<char> stdStringViewConstChar("String View");
#endif
	}
	end_test();

	//-------
	// string
	//-------

	begin_test("string");
	{
		crstl::string crStringEmpty;
		crstl::wstring crWstringEmpty;
		std::string stdStringEmpty;
		crstl_check(crStringEmpty.size() == stdStringEmpty.size());
		crstl_check(crWstringEmpty.size() == stdStringEmpty.size());

		if (crStringEmpty.empty())
		{
			crStringEmpty.clear();
		}

		crStringEmpty.reserve(32);
		stdStringEmpty.reserve(32);

		crstl::string crStringSSO("Hello");
		crstl::wstring crWstringSSO(L"Hello");
		std::string stdStringSSO("Hello");
		crstl_check(crStringSSO.size() == stdStringSSO.size());
		crstl_check(crWstringSSO.size() == stdStringSSO.size());

		crstl::string crStringHeap("Hello string my old friend");
		crstl::wstring crWstringHeap(L"Hello string my old friend");
		std::string stdStringHeap("Hello string my old friend");
		crstl_check(crStringEmpty.size() == stdStringEmpty.size());
		crstl_check(crWstringEmpty.size() == stdStringEmpty.size());

		//std::string perry(4);
		crstl::string crStringInteger(462);
		crstl::string crStringFloat(462.0f);

		// append

		crstl::string crStringAppend;
		crstl::wstring crWstringAppend;
		std::string stdStringAppend;

		stdStringAppend.append("Hello string");
		stdStringAppend.append(", my old friend");
		stdStringAppend.append(", when will you");
		stdStringAppend.append(" allocate?");

		crStringAppend.append("Hello string");
		crStringAppend.append(", my old friend");
		crStringAppend.append(", when will you");
		crStringAppend.append(" allocate?");

		crstl_check(crStringAppend.size() == stdStringAppend.size());

		crWstringAppend.append(L"Hello string");
		crWstringAppend.append(L", my old friend");
		crWstringAppend.append(L", when will you");
		crWstringAppend.append(L" allocate?");
		
		crstl_check(crWstringAppend.size() == stdStringAppend.size());

		crStringAppend.append(3, '!');
		stdStringAppend.append(3, '!');
		crstl_check(crStringAppend.size() == stdStringAppend.size());

		// append_convert

		crstl::string crStringAppendConvert;
		crstl::wstring crWStringAppendConvert;

		crStringAppendConvert.append("Hello ");
		crStringAppendConvert.append_convert(TaberuWchar);
		crStringAppendConvert.append_convert(L"\u98df\u98df\u98df\u98df");
		crStringAppendConvert.append_convert(L"Now we add a very long string to a string we know already is heap allocated");

		crWStringAppendConvert.append_convert(u8"\u98df");

		//crStringAppendConvert.append_convert(L", my old friend");
		//crStringAppendConvert.append_convert(L", when will you");
		//crStringAppendConvert.append_convert(L" allocate?");

		// append_sprintf

		crstl::string crStringAppendSprintf;
		crstl::wstring crWstringSprintf;

		crStringAppendSprintf.append_sprintf("Hello %s", "Incredibly Complex World!");

		// assign

		crstl::string crStringAssign;
		crstl::wstring crWstringAssign;
		std::string stdStringAssign;

		crStringAssign.assign("Hello String!");
		crWstringAssign.assign(L"Hello String!");
		stdStringAssign.assign("Hello String!");

		crstl_check(crStringAssign.size() == stdStringAssign.size());
		crstl_check(crWstringAssign.size() == stdStringAssign.size());

		crStringAssign.assign(HelloString);
		crWstringAssign.assign(WHelloString);
		stdStringAssign.assign(HelloString);

		crstl_check(crStringAssign.size() == stdStringAssign.size());
		crstl_check(crWstringAssign.size() == stdStringAssign.size());

		// erase
		{
			crstl::string crStringErase = "Hello String";
			std::string stdStringErase = "Hello String";
		
			crStringErase.erase(2, 6);
			stdStringErase.erase(2, 6);
		}

		// replace
		{
			crstl::string crStringReplace = "Hello String";
			std::string stdStringReplace = "Hello String";

			crStringReplace.replace(6, 6, "World", crstl::string_length("World"));
			stdStringReplace.replace(6, 6, "World", crstl::string_length("World"));
			crstl_check(crStringReplace.size() == stdStringReplace.size());

			crStringReplace.replace(6, 1, 20, 'k');
			stdStringReplace.replace(6, 1, 20, 'k');
			crstl_check(crStringReplace.size() == stdStringReplace.size());
		}

		// reserve
		{
			crstl::string crStringReserve;
			crStringReserve.reserve(100);
		}

		// resize

		crstl::string crStringResize;
		crstl::wstring crWstringResize;
		std::string stdStringResize;

		crStringResize.resize(16, 'k');
		crWstringResize.resize(16, L'k');
		stdStringResize.resize(16, 'k');

		crstl_check(crStringResize.size() == stdStringResize.size());
		crstl_check(crWstringResize.size() == stdStringResize.size());

		// operator +

		crstl::string crStringExample1("Hello ");
		crstl::string crStringExample2("String");
		
		crstl::string crStringConcat1 = crStringExample1 + crStringExample2;
		crstl::string crStringConcat2 = crStringExample1 + "String";
		crstl::string crStringConcat3 = crStringExample1 + StringArray;
		crstl::string crStringConcat4 = crStringExample1 + String;

		crstl::string crStringConcat5 = "Hello " + crStringExample2;
		crstl::string crStringConcat6 = HelloArray + crStringExample2;
		crstl::string crStringConcat7 = Hello + crStringExample2;

		// move
		{
			crstl::string crStringLongString = "Here is a long string that triggers the heap";
			crstl::string crStringCopiedString = crStringLongString;
			crStringCopiedString = crStringLongString;
			crstl::string crStringMovedString = crstl::move(crStringLongString);
		}
	}
	end_test();

	// Unicode decoding

#if defined(CRSTL_UNIT_UNICODE_LITERALS)
	const char* MyCharString = (const char*)u8"Hello String汉字";
	const wchar_t* MyWCharString = L"Hello String汉字";

	const char* u1 = (const char*)u8"a";
	const char* u2 = (const char*)u8"\u03EA";
	const char* u3 = (const char*)u8"\u27c1";
	const char* u4 = (const char*)u8"\U00010CFF";

	size_t utf8Offset = 0;
	crstl::codepoint_t cp = crstl::decode_utf8((const uint8_t*)u4, strlen((const char*)u4), utf8Offset);
#else
	const char* MyCharString = (const char*)"Hello String汉字";
	const wchar_t* MyWCharString = L"Hello String汉字";
#endif
}