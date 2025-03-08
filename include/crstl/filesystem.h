#pragma once

#include "crstl/config.h"
#include "crstl/crstldef.h"
#include "crstl/path.h"
#include "crstl/fixed_path.h"
#include "crstl/move_forward.h"

crstl_module_export namespace crstl
{
	// Set a reasonable maximum across all possible OSs
	static const uint32_t kMaxPathLength = 1024;

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

	namespace file_copy_options
	{
		enum t
		{
			none = 0 << 0,
			overwrite = 1 << 0
		};
	};

	enum class file_seek_origin
	{
		begin,
		current,
		end,
	};

	enum class filesystem_result
	{
		success,
		error,
		error_not_found,
		error_access_denied
	};

	struct directory_entry
	{
		const char* directory;
		const char* filename;
		bool is_directory;
	};

	class file_base
	{
	public:

		const path& get_path() const
		{
			return m_path;
		}

		file_flags::t get_flags() const
		{
			return m_flags;
		}

	protected:

		// Don't try to instantiate a file_base directly

		file_base() : m_flags(file_flags::none) {}

		file_base(file_flags::t flags) : m_flags(flags) {}

		file_base(file_base&& other) crstl_noexcept
		{
			m_path = crstl_move(other.m_path);

			m_flags = other.m_flags;
			other.m_flags = file_flags::none;
		}

		file_base& operator = (file_base&& other) crstl_noexcept
		{
			m_path = crstl_move(other.m_path);

			m_flags = other.m_flags;
			other.m_flags = file_flags::none;

			return *this;
		}

		file_flags::t m_flags;

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
		// Path to a directory suitable for temporary files
		static const path temp_path;

		// Path to the executable, including filename
		static const path executable_path;
	};

	template<typename Dummy>
	const path filesystem_globals<Dummy>::temp_path = detail::compute_temp_path();

	// https://stackoverflow.com/questions/1528298/get-path-of-executable
	// https://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
	template<typename Dummy>
	const path filesystem_globals<Dummy>::executable_path = detail::compute_executable_path();
}

crstl_module_export namespace crstl
{
	inline const path& temp_directory_path()
	{
		return filesystem_globals<void>::temp_path;
	}

	inline const path& executable_path()
	{
		return filesystem_globals<void>::executable_path;
	}

	// Create directory, including the tree that leads up to it
	// If folder was created successfully or folder already exists, return true
	inline bool create_directories(const char* directory_path)
	{
		if (!directory_path)
		{
			return false;
		}

		filesystem_result result = create_directory(directory_path);

		if (result == filesystem_result::success)
		{
			return true;
		}
		else
		{
			// We guarantee path separators are only ever represented with forward slashes in the path class
			const char* Separator = "/";

			// Start working our way backwards. Chances are most of the path exists
			crstl::path temp_path = directory_path;
			size_t separator_pos = temp_path.find_last_of(Separator);

			// Skip trailing slash
			if (separator_pos == temp_path.length() - 1)
			{
				separator_pos = temp_path.find_last_of(Separator, separator_pos - 1);
			}

			while (separator_pos != crstl::path::npos)
			{
				temp_path[separator_pos] = 0;
				bool subdirectory_exists = exists(temp_path.c_str());
				temp_path[separator_pos] = '/';

				// If the subdirectory exists, we now need to move forward creating the rest
				if (subdirectory_exists)
				{
					separator_pos = temp_path.find_first_of(Separator, separator_pos + 1);
					break;
				}
				else
				{
					separator_pos = temp_path.find_last_of(Separator, separator_pos - 1);
				}
			}

			bool all_paths_created = false;

			// All the paths from here should be correctly created. If any one path fails,
			// we need to assume there is an error
			while (separator_pos != crstl::path::npos)
			{
				temp_path[separator_pos] = 0;
				filesystem_result subdirectory_created = create_directory(temp_path.c_str());
				temp_path[separator_pos] = '/';

				if (subdirectory_created == filesystem_result::success)
				{
					separator_pos = temp_path.find_first_of(Separator, separator_pos + 1);
					all_paths_created |= true;
				}
				else
				{
					all_paths_created = false;
					break;
				}
			}

			return all_paths_created;
		}
	}
}