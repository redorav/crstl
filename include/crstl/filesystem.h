#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/path.h"

crstl_module_export namespace crstl
{
	// Set a reasonable maximum across all possible OSs
	static const uint32_t MaxPathLength = 1024;

	namespace file_flags
	{
		enum t : uint32_t
		{
			none         = 0,
			read         = 1 << 0, // Read from the file
			write        = 1 << 1, // Write to the file
			append       = 1 << 2, // Append to the file
			create       = 1 << 3, // Create file if it does not exist
			force_create = 1 << 4, // Always create file, overwriting previous contents
		};

		inline file_flags::t operator | (file_flags::t a, file_flags::t b)
		{
			return static_cast<file_flags::t>((uint32_t)a | (uint32_t)b);
		}
	};

	namespace file_seek_origin
	{
		enum t
		{
			begin,
			current,
			end,
		};
	};

	namespace filesystem_result
	{
		enum t
		{
			success,
			error,
			error_not_found,
			error_access_denied
		};

		struct conversion
		{
			conversion(filesystem_result::t result) : result(result) {}
			operator bool() { return result == filesystem_result::success; }
			filesystem_result::t result;
		};
	};

	namespace file_copy_options
	{
		enum t
		{
			none      = 0 << 0,
			overwrite = 1 << 0
		};
	};

	class file_base
	{
	public:

		const path& get_path() const
		{
			return m_path;
		}

	protected:

		path m_path;
	};
};

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/filesystem_win32.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_ANDROID) || defined(CRSTL_OS_OSX)
#include "crstl/platform/filesystem_posix.h"
#endif

namespace crstl
{
	// These only get initialized once

	template<typename Dummy>
	struct filesystem_globals
	{
		static const path temp_path;
	};

	template<typename Dummy>
	const path filesystem_globals<Dummy>::temp_path = detail::compute_temp_path();

	inline const path& temp_directory_path()
	{
		return filesystem_globals<void>::temp_path;
	}
}