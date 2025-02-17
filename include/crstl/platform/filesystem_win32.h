#pragma once

#include "common_win32.h"

crstl_module_export namespace crstl
{
	namespace detail
	{
		inline void win32_utf8_to_utf16(const CHAR* path, size_t path_length, WCHAR* destination_path, size_t destination_path_length)
		{
			int end_position = MultiByteToWideChar(CRSTL_CP_UTF8, 0, path, (int)path_length, destination_path, (int)destination_path_length);
			destination_path[end_position] = L'\0';
		}

		inline void win32_utf16_to_utf8(const WCHAR* path, size_t path_length, CHAR* destination_path, size_t destination_path_length)
		{
			int end_position = WideCharToMultiByte(CRSTL_CP_UTF8, 0, path, (int)path_length, destination_path, (int)destination_path_length, nullptr, nullptr);
			destination_path[end_position] = '\0';
		}
	}

	class file : public file_base
	{
	public:

		file() crstl_noexcept
			: file_base()
			, m_file_handle(CRSTL_INVALID_HANDLE_VALUE)
		{}

		file(const char* file_path, file_flags::t open_flags) crstl_noexcept
			: file_base(open_flags)
			, m_file_handle(CRSTL_INVALID_HANDLE_VALUE)
		{
			DWORD dw_desired_access = 0;
			DWORD dw_share_mode = 0;
			LPSECURITY_ATTRIBUTES lp_security_attributes = nullptr;
			DWORD dw_creation_disposition = 0;
			DWORD dw_flags_and_attributes = 0;
			HANDLE h_template_file = nullptr;

			if (open_flags & file_flags::read)
			{
				dw_desired_access |= CRSTL_GENERIC_READ;
			}

			if (open_flags & file_flags::write)
			{
				dw_desired_access |= CRSTL_GENERIC_WRITE;
			}

			// https://stackoverflow.com/questions/14469607/difference-between-open-always-and-create-always-in-createfile-of-windows-api
			if (open_flags & file_flags::create)
			{
				dw_creation_disposition = CRSTL_OPEN_ALWAYS;
			}
			else if (open_flags & file_flags::force_create)
			{
				dw_creation_disposition = CRSTL_CREATE_ALWAYS;
			}
			else
			{
				dw_creation_disposition = CRSTL_OPEN_EXISTING;
			}

			// Allow other processes to access this file, but only for reading
			dw_share_mode = CRSTL_FILE_SHARE_READ;

			dw_flags_and_attributes = CRSTL_FILE_ATTRIBUTE_NORMAL;

			if (detail::win32_is_utf8())
			{
				m_file_handle = ::CreateFileA(file_path, dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
			}
			else
			{
				WCHAR w_file_path[MaxPathLength];
				detail::win32_utf8_to_utf16(file_path, crstl::string_length(file_path), w_file_path, sizeof(w_file_path));
				m_file_handle = ::CreateFileW(w_file_path, dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
			}

			if (is_open())
			{
				m_path = file_path;

				if (!(open_flags & file_flags::append))
				{
					rewind();
				}
			}
		}
		
		~file()
		{
			close();
		}

		file(file&& other) crstl_noexcept : file_base(crstl_move(other))
		{
			crstl_assert(this != &other);

			m_file_handle = other.m_file_handle;
			other.m_file_handle = CRSTL_INVALID_HANDLE_VALUE;
		}

		file& operator = (file&& other) crstl_noexcept
		{
			crstl_assert(this != &other);

			close();

			file_base::operator = (crstl_move(other));

			m_file_handle = other.m_file_handle;
			other.m_file_handle = CRSTL_INVALID_HANDLE_VALUE;

			return *this;
		}

		explicit operator bool() const
		{
			return is_open();
		}

		void close()
		{
			if (is_open())
			{
				CloseHandle(m_file_handle);
				m_file_handle = CRSTL_INVALID_HANDLE_VALUE;
			}
		}

		size_t get_size() const
		{
			// Note that if the return value is INVALID_FILE_SIZE (0xffffffff), an application must call GetLastError 
			// to determine whether the function has succeeded or failed. The reason the function may appear to fail 
			// when it has not is that lpFileSizeHigh could be non-NULL or the file size could be 0xffffffff. In this 
			// case, GetLastError will return NO_ERROR (0) upon success. Because of this behavior, it is recommended 
			// that you use GetFileSizeEx instead.

			large_integer file_size;
			GetFileSizeEx(m_file_handle, (PLARGE_INTEGER)&file_size);
			return (size_t)file_size.quad_part;
		}
		
		bool is_open() const
		{
			return m_file_handle != CRSTL_INVALID_HANDLE_VALUE;
		}

		size_t read(void* memory, size_t bytes) const
		{
			crstl_assert(is_open());
			crstl_assert(m_flags & file_flags::read);

			DWORD bytes_read;
			::ReadFile(m_file_handle, memory, (DWORD)bytes, &bytes_read, nullptr);
			return (size_t)bytes_read;
		}

		void rewind()
		{
			crstl_assert(is_open());
			SetFilePointer(m_file_handle, 0, nullptr, CRSTL_FILE_BEGIN);
		}

		void seek(file_seek_origin seek_origin, int64_t byte_offset)
		{
			crstl_assert(is_open());

			large_integer li_byte_offset;
			li_byte_offset.quad_part = byte_offset;

			DWORD dw_move_method = CRSTL_FILE_BEGIN;

			switch (seek_origin)
			{
			case file_seek_origin::begin:
				dw_move_method = CRSTL_FILE_BEGIN;
				break;
			case file_seek_origin::current:
				dw_move_method = CRSTL_FILE_CURRENT;
				break;
			case file_seek_origin::end:
				dw_move_method = CRSTL_FILE_END;
				break;
			}

			DWORD result = SetFilePointer
			(
				m_file_handle,
				li_byte_offset.low_part,
				&li_byte_offset.high_part,
				dw_move_method
			);

			if (result == CRSTL_INVALID_SET_FILE_POINTER)
			{
				// Handle error
			}
		}

		size_t write(const void* memory, size_t bytes)
		{
			crstl_assert(is_open());
			crstl_assert(m_flags & file_flags::write);

			DWORD bytes_written;
			::WriteFile(m_file_handle, memory, (DWORD)bytes, &bytes_written, nullptr);
			return (size_t)bytes_written;
		}

	private:

		file(const file&) crstl_constructor_delete;

		file& operator = (const file&) crstl_constructor_delete;

		HANDLE m_file_handle;
	};

	inline void copy_file(const char* source_file_path, const char* destination_file_path, file_copy_options::t copy_options)
	{
		bool fail_if_exists = (copy_options & file_copy_options::overwrite) ? false : true;

		if (detail::win32_is_utf8())
		{
			CopyFileA(source_file_path, destination_file_path, fail_if_exists);
		}
		else
		{
			WCHAR w_source_file_path[MaxPathLength];
			detail::win32_utf8_to_utf16(source_file_path, crstl::string_length(source_file_path), w_source_file_path, sizeof(w_source_file_path));

			WCHAR w_destination_file_path[MaxPathLength];
			detail::win32_utf8_to_utf16(destination_file_path, crstl::string_length(destination_file_path), w_destination_file_path, sizeof(w_destination_file_path));

			CopyFileW(w_source_file_path, w_destination_file_path, fail_if_exists);
		}

		// TODO ERROR HANDLING
	}

	inline void move_file(const char* source_file_path, const char* destination_file_path)
	{
		if (detail::win32_is_utf8())
		{
			MoveFileA(source_file_path, destination_file_path);
		}
		else
		{
			WCHAR w_source_file_path[MaxPathLength];
			detail::win32_utf8_to_utf16(source_file_path, crstl::string_length(source_file_path), w_source_file_path, sizeof(w_source_file_path));

			WCHAR w_destination_file_path[MaxPathLength];
			detail::win32_utf8_to_utf16(destination_file_path, crstl::string_length(destination_file_path), w_destination_file_path, sizeof(w_destination_file_path));

			/*BOOL result = */MoveFileW(w_source_file_path, w_destination_file_path);
		}

		// TODO ERROR HANDLING
	}

	inline filesystem_result delete_file(const char* file_path)
	{
		bool success = false;

		if (detail::win32_is_utf8())
		{
			success = DeleteFileA(file_path);
		}
		else
		{
			WCHAR w_file_path[MaxPathLength];
			detail::win32_utf8_to_utf16(file_path, crstl::string_length(file_path), w_file_path, sizeof(w_file_path));
			success = DeleteFileW(w_file_path);
		}

		if (success)
		{
			return filesystem_result::success;
		}
		else
		{
			DWORD errorCode = GetLastError();

			// If the file was not found we consider it a success as the file is effectively deleted
			if (errorCode == CRSTL_ERROR_FILE_NOT_FOUND)
			{
				return filesystem_result::success;
			}
			else if (errorCode == CRSTL_ERROR_ACCESS_DENIED || errorCode == CRSTL_ERROR_SHARING_VIOLATION)
			{
				return filesystem_result::error_access_denied;
			}

			return filesystem_result::error;
		}
	}

	inline bool exists(const char* path)
	{
		_win32_file_attribute_data attribute_data;

		bool success = false;

		if (detail::win32_is_utf8())
		{
			success = GetFileAttributesExA(path, (GET_FILEEX_INFO_LEVELS)GetFileExInfoStandard, &attribute_data);
		}
		else
		{
			WCHAR w_file_path[MaxPathLength];
			detail::win32_utf8_to_utf16(path, crstl::string_length(path), w_file_path, sizeof(w_file_path));
			success = GetFileAttributesExW(w_file_path, (GET_FILEEX_INFO_LEVELS)GetFileExInfoStandard, &attribute_data);
		}

		if (success)
		{
			return attribute_data.dwFileAttributes != CRSTL_INVALID_FILE_ATTRIBUTES;
		}
		else
		{
			return false;
		}
	}

	inline filesystem_result create_directory(const char* directory_path)
	{
		bool success = false;

		if (detail::win32_is_utf8())
		{
			success = CreateDirectoryA(directory_path, nullptr);
		}
		else
		{
			WCHAR w_directory_path[MaxPathLength];
			detail::win32_utf8_to_utf16(directory_path, crstl::string_length(directory_path), w_directory_path, sizeof(w_directory_path));
			success = CreateDirectoryW(w_directory_path, nullptr);
		}

		if (success)
		{
			return filesystem_result::success;
		}
		else
		{
			DWORD errorCode = GetLastError();

			if (errorCode == CRSTL_ERROR_ALREADY_EXISTS)
			{
				return filesystem_result::success;
			}
			else
			{
				return filesystem_result::error;
			}
		}
	}

	namespace detail
	{
		inline path compute_temp_path()
		{
			// Start off null-terminated
			char temp_path[MaxPathLength];
			temp_path[0] = 0;

			// Note on GetLongPathName reusing the parameter:
			// From the docs: "You can use the same buffer you used for the lpszShortPath parameter"

			if (win32_is_utf8())
			{
				DWORD short_temp_path_length = GetTempPathA(MaxPathLength, temp_path);
				if (short_temp_path_length > 0)
				{
					GetLongPathNameA(temp_path, temp_path, MaxPathLength);
				}
			}
			else
			{
				WCHAR w_temp_path[MaxPathLength];

				DWORD short_temp_path_length = GetTempPathW(MaxPathLength, w_temp_path);
				if (short_temp_path_length > 0)
				{
					DWORD long_temp_path_length = GetLongPathNameW(w_temp_path, w_temp_path, MaxPathLength);
					if (long_temp_path_length > 0)
					{
						win32_utf16_to_utf8(w_temp_path, crstl::string_length(w_temp_path), temp_path, sizeof(temp_path));
					}
				}
			}

			return path(temp_path);
		}
	}
};
