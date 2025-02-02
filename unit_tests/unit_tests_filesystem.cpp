#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/filesystem.h"
#include "crstl/utility/string_length.h"
#endif

#include <stdio.h>

void RunUnitTestsFilesystem()
{
	printf("RunUnitTestsFilesystem\n");

	using namespace crstl_unit;

	begin_test("filesystem");
	{
		crstl::path temp_path = crstl::get_temp_path() / "crstl_temp";

		crstl::path temp_file_path = temp_path / "temp_file.txt";
		crstl::path temp_file_copy_path = temp_path / "temp_file_copy.txt";
		crstl::path temp_file_move_path = temp_path / "temp_file_move.txt";
		const char* temp_text = "I am a temp text";

		crstl::file_delete(temp_file_path.c_str());
		crstl::file_delete(temp_file_copy_path.c_str());
		crstl::file_delete(temp_file_move_path.c_str());
		crstl_assert(!crstl::file_exists(temp_file_path.c_str()));
		crstl_assert(!crstl::file_exists(temp_file_copy_path.c_str()));
		crstl_assert(!crstl::file_exists(temp_file_move_path.c_str()));

		{
			crstl::file my_file(temp_file_path.c_str(), crstl::file_flags::create | crstl::file_flags::write);

			// Make sure we cannot delete a file that is already open
			crstl::filesystem_result::t delete_result = crstl::file_delete(temp_file_path.c_str());
			crstl_assert(delete_result == crstl::filesystem_result::error_access_denied);

			// Write a bit of text
			my_file.write(temp_text, crstl::string_length(temp_text));

			// File closes when out of scope
		}

		crstl::file_copy(temp_file_path.c_str(), temp_file_copy_path.c_str(), crstl::file_copy_options::none);
		crstl_assert(crstl::file_exists(temp_file_copy_path.c_str()));

		crstl::file_move(temp_file_path.c_str(), temp_file_move_path.c_str());
		crstl_assert(!crstl::file_exists(temp_file_path.c_str()));
		crstl_assert(crstl::file_exists(temp_file_move_path.c_str()));

		if (!crstl::file_exists("C:/temp/temp_file.txt"))
		{

		}
	}
	end_test();
}