#pragma once

#include "crstl/config.h"

crstl_module_export namespace crstl
{
	enum class directory_watcher_action
	{
		none,
		file_created,
		file_deleted,
		file_modified,
		file_renamed_old_name,
		file_renamed_new_name
	};

	struct directory_watcher_parameters
	{
		directory_watcher_parameters()
			: watch_subtree(false)
			, watch_files(true)
			, watch_directories(true)
		{}

		bool watch_subtree;
		bool watch_files;
		bool watch_directories;
	};

	class directory_watcher_base
	{
	public:

		directory_watcher_base() {}

		directory_watcher_base(const directory_watcher_parameters& parameters) : m_parameters(parameters) {}

	protected:

		directory_watcher_parameters m_parameters;
	};

	struct directory_modified_entry
	{
		directory_watcher_action action;
		char* filename;
	};
}

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/directory_watcher_win32.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_ANDROID)
#include "crstl/platform/directory_watcher_linux.h"
#endif