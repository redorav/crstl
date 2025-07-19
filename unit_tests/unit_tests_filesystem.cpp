#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/filesystem.h"
#include "crstl/directory_watcher.h"
#include "crstl/utility/string_length.h"
#endif

#include <stdio.h>

void RunUnitTestsFilesystem()
{
	printf("RunUnitTestsFilesystem\n");

	using namespace crstl_unit;

	begin_test("filesystem");
	{
		crstl::path temp_path = crstl::temp_directory_path() / "crstl_temp";

		crstl::path temp_file_path = temp_path / "temp_file.txt";
		crstl::path temp_file_copy_path = temp_path / "temp_file_copy.txt";
		crstl::path temp_file_move_path = temp_path / "temp_file_move.txt";
		const char* temp_text = "I am a temp text";

		crstl::delete_file(temp_file_path.c_str());
		crstl::delete_file(temp_file_copy_path.c_str());
		crstl::delete_file(temp_file_move_path.c_str());
		crstl_assert(!crstl::exists(temp_file_path.c_str()));
		crstl_assert(!crstl::exists(temp_file_copy_path.c_str()));
		crstl_assert(!crstl::exists(temp_file_move_path.c_str()));

		{
			crstl::file my_file(temp_file_path.c_str(), crstl::file_flags::create | crstl::file_flags::write);

			// Make sure we cannot delete a file that is already open
			crstl::filesystem_result delete_result = crstl::delete_file(temp_file_path.c_str());
			crstl_assert(delete_result == crstl::filesystem_result::error_access_denied);

			// Write a bit of text
			my_file.write(temp_text, crstl::string_length(temp_text));

			// File closes when out of scope
		}

		crstl::copy_file(temp_file_path.c_str(), temp_file_copy_path.c_str(), crstl::file_copy_options::none);
		crstl_assert(crstl::exists(temp_file_copy_path.c_str()));

		crstl::move_file(temp_file_path.c_str(), temp_file_move_path.c_str());
		crstl_assert(!crstl::exists(temp_file_path.c_str()));
		crstl_assert(crstl::exists(temp_file_move_path.c_str()));

		if (!crstl::exists("C:/temp/temp_file.txt"))
		{

		}
	}
	end_test();

	begin_test("directory_watcher");
	{
		crstl::path temp_path = crstl::temp_directory_path() / "crstl_directory_watcher_temp";
		crstl::create_directories(temp_path.c_str());

		crstl::path dummy_file_path = temp_path / "my_file.txt";

		// Ensure file doesn't exist before we start the file watching process
		crstl::delete_file(dummy_file_path.c_str());

		crstl::directory_watcher_parameters parameters;
		parameters.watch_subtree = true;
		crstl::directory_watcher my_watcher(temp_path.c_str(), parameters);

		uint32_t operations = 0;

		while (operations < 2)
		{
			if (operations == 0)
			{
				crstl::file my_file(dummy_file_path.c_str(), crstl::file_flags::create);
			}
			else if (operations == 1)
			{
				crstl::delete_file(dummy_file_path.c_str());
			}

			my_watcher.update_sync([&operations](const crstl::directory_modified_entry& entry)
			{
				const char* operationText = "";
				switch (entry.action)
				{
					case crstl::directory_watcher_action::file_created: operationText = "created"; break;
					case crstl::directory_watcher_action::file_modified: operationText = "modified"; break;
					case crstl::directory_watcher_action::file_deleted: operationText = "deleted"; break;
					case crstl::directory_watcher_action::file_renamed_new_name: operationText = "renamed new name"; break;
					case crstl::directory_watcher_action::file_renamed_old_name: operationText = "renamed old name"; break;
				}

				printf("Entry %s was %s\n", entry.filename, operationText);

				operations++;
			});

			crstl::this_thread::sleep_for(1000);
		}
	}
	end_test();
}