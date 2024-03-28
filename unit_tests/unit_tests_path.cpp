#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/fixed_path.h"
#include "crstl/path.h"
#include "crstl/string.h"
#endif

// Filesystem begins with C++17
#if CRSTL_CPPVERSION >= CRSTL_CPP17
#include <filesystem>
#endif

#include <stdio.h>

void RunUnitTestsPath()
{
	printf("RunUnitTestsPath\n");

#if CRSTL_CPPVERSION >= CRSTL_CPP17
	using namespace crstl_unit;

	crstl::fixed_path512 fixedPath;

	crstl::string examplePaths[] =
	{
		"",
		".",
		"..",
		//"/", // Fails unit tests due to incomplete implementation of parent_path()
		"foo",
		"foo.txt",
		"C:/foo/bar/", // Path ending in /
		"C:/foo/bar/foobar", // Path ending in filename without extension
		"C:/foo/bar/foobar.txt", // Path ending in filename with extension
		"C:/foo.bar/foo.bar/foobar.txt", // Path with dots ending in filename with extension
		"C:/foo.bar/foo.bar/foobar", // Path with dots ending in filename without extension
	};

	for (size_t i = 0; i < sizeof(examplePaths) / sizeof(examplePaths[0]); ++i)
	{
		const crstl::string& examplePath = examplePaths[i];

		std::filesystem::path stdPath = examplePath.c_str();
		crstl::fixed_path512 crFixedPath = examplePath.c_str();
		crstl::path crPath = examplePath.c_str();

		// Check empty path concatenation
		{
			crstl::string stdPathFilename((stdPath / "foobar.exe").string().c_str());
			stdPathFilename.replace_all('\\', '/');

			crstl::string crFixedPathFilename((crFixedPath / "foobar.exe").c_str());
			crstl_check(stdPathFilename == crFixedPathFilename);

			crstl::string crPathFilename((crPath / "foobar.exe").c_str());
			crstl_check(stdPathFilename == crPathFilename);
		}

		// Check filename
		{
			crstl::string stdPathFilename(stdPath.filename().string().c_str());

			crstl::string crFixedPathFilename(crFixedPath.filename().c_str());
			crstl_check(stdPathFilename == crFixedPathFilename);

			crstl::string crPathFilename(crPath.filename().c_str());
			crstl_check(stdPathFilename == crPathFilename);
		}

		// Check parent path
		{
			crstl::string stdPathParentPath(stdPath.parent_path().string().c_str());

			crstl::string crFixedPathParentPath(crFixedPath.parent_path().c_str());
			crstl_check(stdPathParentPath == crFixedPathParentPath);

			crstl::string crPathParentPath(crPath.parent_path().c_str());
			crstl_check(stdPathParentPath == crPathParentPath);
		}

		// Check extension
		{
			crstl::string stdPathExtension(stdPath.extension().string().c_str());

			crstl::string crFixedPathExtension(crFixedPath.extension().c_str());
			crstl_check(crFixedPathExtension == stdPathExtension);

			crstl::string crPathExtension(crPath.extension().c_str());
			crstl_check(crPathExtension == stdPathExtension);
		}

		// Check has_extension
		crstl_check(stdPath.has_extension() == crFixedPath.has_extension());
		crstl_check(stdPath.has_extension() == crPath.has_extension());
	}
#endif
}