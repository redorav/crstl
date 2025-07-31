#pragma once

// References
// https://gist.github.com/nickav/a57009d4fcc3b527ed0f5c9cf30618f8

#include "crstl/type_array.h"

#include "crstl/platform/common_win32.h"

crstl_module_export namespace crstl
{
	class directory_watcher : public directory_watcher_base
	{
	public:

		directory_watcher()
			: m_directory_handle(CRSTL_INVALID_HANDLE_VALUE)
			, m_event_handle(CRSTL_INVALID_HANDLE_VALUE)
			, m_buffer(nullptr)
			, m_is_monitoring(false)
		{}

		directory_watcher(const char* directory_path, const directory_watcher_parameters& parameters)
			: directory_watcher_base(parameters)
			, m_directory_handle(CRSTL_INVALID_HANDLE_VALUE)
			, m_event_handle(CRSTL_INVALID_HANDLE_VALUE)
			, m_buffer(nullptr)
			, m_is_monitoring(false)
		{
			DWORD access_rights        = CRSTL_FILE_LIST_DIRECTORY;
			DWORD share_mode           = CRSTL_FILE_SHARE_READ | CRSTL_FILE_SHARE_WRITE | CRSTL_FILE_SHARE_DELETE;
			DWORD creation_disposition = CRSTL_OPEN_EXISTING;
			DWORD backup_semantics     = CRSTL_FILE_FLAG_BACKUP_SEMANTICS | CRSTL_FILE_FLAG_OVERLAPPED;
			HANDLE template_file       = nullptr;

			if (detail::win32_is_utf8())
			{
				m_directory_handle = CreateFileA
				(
					directory_path,
					access_rights,
					share_mode,
					nullptr,
					creation_disposition,
					backup_semantics,
					template_file
				);
			}
			else
			{
				WCHAR w_file_path[kMaxPathLength];
				detail::win32_utf8_to_utf16(directory_path, crstl::string_length(directory_path), w_file_path, crstl::array_size(w_file_path));
				m_directory_handle = CreateFileW
				(
					w_file_path,
					access_rights,
					share_mode,
					nullptr,
					creation_disposition,
					backup_semantics,
					template_file
				);
			}

			if (m_directory_handle != CRSTL_INVALID_HANDLE_VALUE)
			{
				m_event_handle = CreateEventA(nullptr, false, false, nullptr);

				try_read_directory_changes();
			}
		}

		~directory_watcher()
		{
			stop();

			if (m_buffer)
			{
				delete[] m_buffer;
			}
		}

		bool is_watching() const
		{
			return m_directory_handle != CRSTL_INVALID_HANDLE_VALUE;
		}

		void stop()
		{
			CloseHandle(m_directory_handle);
			CloseHandle(m_event_handle);
			m_directory_handle = CRSTL_INVALID_HANDLE_VALUE;
			m_event_handle = CRSTL_INVALID_HANDLE_VALUE;
		}

		template<typename DirectoryUpdateCallback>
		void update_sync(const DirectoryUpdateCallback& callback)
		{
			if (is_watching())
			{
				try_read_directory_changes();

				//crstl_assert(result != ERROR_NOTIFY_ENUM_DIR)

				DWORD wait_status = WaitForSingleObject(m_event_handle, 0);

				// If wait was successful, process the changed files
				if (wait_status == 0)
				{
					detail::FILE_NOTIFY_INFORMATION* notify_info = nullptr;
					
					DWORD current_offset = 0;

					do
					{
						notify_info = (detail::FILE_NOTIFY_INFORMATION*)(m_buffer + current_offset);

						char filename[kMaxPathLength];
						int filename_length = detail::win32_utf16_to_utf8(notify_info->FileName, notify_info->FileNameLength / 2, filename, crstl::array_size(filename));
						filename[filename_length] = '\0';

						// Replace all backslashes with forward slashes
						for (int i = 0; i < filename_length; ++i)
						{
							if (filename[i] == '\\')
							{
								filename[i] = '/';
							}
						}

						directory_watcher_action watcher_action = directory_watcher_action::none;

						switch (notify_info->Action)
						{
							case CRSTL_FILE_ACTION_ADDED:            // The file is added to the directory
								watcher_action = directory_watcher_action::file_created; break;
							case CRSTL_FILE_ACTION_REMOVED:          // The file is removed from the directory
								watcher_action = directory_watcher_action::file_deleted; break;
							case CRSTL_FILE_ACTION_MODIFIED:         // The file is modified. This can be a change in the time stamp or attributes
								watcher_action = directory_watcher_action::file_modified; break;
							case CRSTL_FILE_ACTION_RENAMED_OLD_NAME: // The file was renamed and this is the old name
								watcher_action = directory_watcher_action::file_renamed_old_name; break;
							case CRSTL_FILE_ACTION_RENAMED_NEW_NAME: // The file was renamed and this is the new name
								watcher_action = directory_watcher_action::file_renamed_new_name; break;
							default:
								break;
						}

						if (watcher_action != directory_watcher_action::none)
						{
							directory_modified_entry modified_entry;
							modified_entry.action   = watcher_action;
							modified_entry.filename = filename;
							callback(modified_entry);
						}

						current_offset += notify_info->NextEntryOffset;
					}
					while (notify_info->NextEntryOffset);

					ResetEvent(m_event_handle);
					
					m_is_monitoring = false;
				}
			}
		}

	private:

		void try_read_directory_changes()
		{
			uint32_t buffer_size = 1024 * 16;

			if (!m_buffer)
			{
				m_buffer = (char*) new char[buffer_size]();
			}

			DWORD notify_filter =
				CRSTL_FILE_NOTIFY_CHANGE_FILE_NAME |
				CRSTL_FILE_NOTIFY_CHANGE_DIR_NAME |
				CRSTL_FILE_NOTIFY_CHANGE_ATTRIBUTES |
				CRSTL_FILE_NOTIFY_CHANGE_SIZE |
				CRSTL_FILE_NOTIFY_CHANGE_LAST_WRITE |
				CRSTL_FILE_NOTIFY_CHANGE_LAST_ACCESS |
				CRSTL_FILE_NOTIFY_CHANGE_CREATION |
				CRSTL_FILE_NOTIFY_CHANGE_SECURITY;

			if (!m_is_monitoring)
			{
				DWORD bytes_written = 0;

				detail::OVERLAPPED o;
				o.hEvent = m_event_handle;

				BOOL result = ReadDirectoryChangesW
				(
					m_directory_handle,
					m_buffer,
					buffer_size,
					m_parameters.watch_subtree,
					notify_filter,
					&bytes_written,
					(LPOVERLAPPED)&o,
					nullptr
				);

				if (result != 0)
				{
					m_is_monitoring = true;
				}
			}
		}

		HANDLE m_directory_handle;

		HANDLE m_event_handle;

		char* m_buffer;

		bool m_is_monitoring;
	};
}