#pragma once

#include "common_win32.h"

// Winnt.h
#define CRSTL_GENERIC_READ                     (0x80000000L)
#define CRSTL_GENERIC_WRITE                    (0x40000000L)
#define CRSTL_GENERIC_EXECUTE                  (0x20000000L)
#define CRSTL_GENERIC_ALL                      (0x10000000L)

#define CRSTL_FILE_SHARE_READ                      0x00000001
#define CRSTL_FILE_SHARE_WRITE                     0x00000002
#define CRSTL_FILE_SHARE_DELETE                    0x00000004
#define CRSTL_FILE_ATTRIBUTE_READONLY              0x00000001
#define CRSTL_FILE_ATTRIBUTE_HIDDEN                0x00000002
#define CRSTL_FILE_ATTRIBUTE_SYSTEM                0x00000004
#define CRSTL_FILE_ATTRIBUTE_DIRECTORY             0x00000010
#define CRSTL_FILE_ATTRIBUTE_ARCHIVE               0x00000020
#define CRSTL_FILE_ATTRIBUTE_DEVICE                0x00000040
#define CRSTL_FILE_ATTRIBUTE_NORMAL                0x00000080
#define CRSTL_FILE_ATTRIBUTE_TEMPORARY             0x00000100
#define CRSTL_FILE_ATTRIBUTE_SPARSE_FILE           0x00000200
#define CRSTL_FILE_ATTRIBUTE_REPARSE_POINT         0x00000400
#define CRSTL_FILE_ATTRIBUTE_COMPRESSED            0x00000800
#define CRSTL_FILE_ATTRIBUTE_OFFLINE               0x00001000
#define CRSTL_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED   0x00002000
#define CRSTL_FILE_ATTRIBUTE_ENCRYPTED             0x00004000
#define CRSTL_FILE_ATTRIBUTE_INTEGRITY_STREAM      0x00008000
#define CRSTL_FILE_ATTRIBUTE_VIRTUAL               0x00010000
#define CRSTL_FILE_ATTRIBUTE_NO_SCRUB_DATA         0x00020000
#define CRSTL_FILE_ATTRIBUTE_EA                    0x00040000
#define CRSTL_FILE_ATTRIBUTE_PINNED                0x00080000
#define CRSTL_FILE_ATTRIBUTE_UNPINNED              0x00100000
#define CRSTL_FILE_ATTRIBUTE_RECALL_ON_OPEN        0x00040000
#define CRSTL_FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000

// Fileapi.h
#define CRSTL_CREATE_NEW          1
#define CRSTL_CREATE_ALWAYS       2
#define CRSTL_OPEN_EXISTING       3
#define CRSTL_OPEN_ALWAYS         4
#define CRSTL_TRUNCATE_EXISTING   5

#define CRSTL_INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define CRSTL_INVALID_SET_FILE_POINTER ((DWORD)-1)
#define CRSTL_INVALID_FILE_ATTRIBUTES ((DWORD)-1)

// Winbase.h
#define CRSTL_FILE_BEGIN           0
#define CRSTL_FILE_CURRENT         1
#define CRSTL_FILE_END             2

// Winerror.h
#define CRSTL_ERROR_FILE_NOT_FOUND             2L
#define CRSTL_ERROR_ACCESS_DENIED              5L
#define CRSTL_ERROR_SHARING_VIOLATION          32L
#define CRSTL_ERROR_ALREADY_EXISTS             183L

crstl_module_export namespace crstl
{
	class file
	{
	public:

		file(const char* file_path, file_flags::t open_flags)
			: m_file_handle(CRSTL_INVALID_HANDLE_VALUE)
			, m_file_flags(open_flags)
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

			m_file_handle = ::CreateFileA
			(
				file_path,
				dw_desired_access,
				dw_share_mode,
				lp_security_attributes,
				dw_creation_disposition,
				dw_flags_and_attributes,
				h_template_file
			);

			if (m_file_handle != CRSTL_INVALID_HANDLE_VALUE)
			{
				// Note that if the return value is INVALID_FILE_SIZE (0xffffffff), an application must call GetLastError 
				// to determine whether the function has succeeded or failed. The reason the function may appear to fail 
				// when it has not is that lpFileSizeHigh could be non-NULL or the file size could be 0xffffffff. In this 
				// case, GetLastError will return NO_ERROR (0) upon success. Because of this behavior, it is recommended 
				// that you use GetFileSizeEx instead.
				long long liFileSize;
				GetFileSizeEx(m_file_handle, (LARGE_INTEGER*)&liFileSize);

				// todo use
				//uint64_t fileSize = liFileSize;

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
		
		bool is_open() const
		{
			return m_file_handle != CRSTL_INVALID_HANDLE_VALUE;
		}

		size_t read(void* memory, size_t bytes)
		{
			crstl_assert(is_open());
			crstl_assert(m_file_flags & file_flags::read);

			DWORD bytes_read;
			::ReadFile(m_file_handle, memory, (DWORD)bytes, &bytes_read, nullptr);
			return (size_t)bytes_read;
		}

		size_t write(const void* memory, size_t bytes)
		{
			crstl_assert(is_open());
			crstl_assert(m_file_flags & file_flags::write);

			DWORD bytes_written;
			::WriteFile(m_file_handle, memory, (DWORD)bytes, &bytes_written, nullptr);
			return (size_t)bytes_written;
		}

		void close()
		{
			if (is_open())
			{
				CloseHandle(m_file_handle);
				m_file_handle = CRSTL_INVALID_HANDLE_VALUE;
			}
		}

		void seek(file_seek_origin::t seek_origin, int64_t byte_offset)
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

		void rewind()
		{
			crstl_assert(is_open());
			SetFilePointer(m_file_handle, 0, nullptr, CRSTL_FILE_BEGIN);
		}

	private:

		file_flags::t m_file_flags;

		HANDLE m_file_handle;
	};

	inline void file_copy(const char* source_file_path, const char* destination_file_path, file_copy_options::t copy_options)
	{
		bool fail_if_exists = copy_options & file_copy_options::overwrite ? false : true;

		// TODO ERROR HANDLING
		CopyFileA(source_file_path, destination_file_path, fail_if_exists);
	}

	inline void file_move(const char* source_file_path, const char* destination_file_path)
	{
		/*BOOL result = */MoveFileA(source_file_path, destination_file_path);
	}

	inline filesystem_return::t file_delete(const char* file_path)
	{
		bool success = DeleteFileA(file_path);

		if (success)
		{
			return filesystem_return::success;
		}
		else
		{
			DWORD errorCode = GetLastError();

			// If the file was not found we consider it a success as the file is effectively deleted
			if (errorCode == CRSTL_ERROR_FILE_NOT_FOUND)
			{
				return filesystem_return::success;
			}
			else if (errorCode == CRSTL_ERROR_ACCESS_DENIED || errorCode == CRSTL_ERROR_SHARING_VIOLATION)
			{
				return filesystem_return::error_access_denied;
			}

			return filesystem_return::error;
		}
	}

	inline bool file_exists(const char* file_path)
	{
		_win32_file_attribute_data attribute_data;

		if (GetFileAttributesExA(file_path, (GET_FILEEX_INFO_LEVELS)GetFileExInfoStandard, &attribute_data))
		{
			return attribute_data.dwFileAttributes != CRSTL_INVALID_FILE_ATTRIBUTES && !(attribute_data.dwFileAttributes & CRSTL_FILE_ATTRIBUTE_DIRECTORY);
		}
		else
		{
			return false;
		}
	}

	inline bool directory_exists(const char* directory_path)
	{
		_win32_file_attribute_data attribute_data;

		if (GetFileAttributesExA(directory_path, (GET_FILEEX_INFO_LEVELS)GetFileExInfoStandard, &attribute_data))
		{
			return attribute_data.dwFileAttributes != CRSTL_INVALID_FILE_ATTRIBUTES && (attribute_data.dwFileAttributes & CRSTL_FILE_ATTRIBUTE_DIRECTORY);
		}
		else
		{
			return false;
		}
	}

	inline filesystem_return::t directory_create(const char* directory_path, bool /*create_intermediate*/)
	{
		bool success = CreateDirectoryA(directory_path, nullptr);

		if (success)
		{
			return filesystem_return::success;
		}
		else
		{
			DWORD errorCode = GetLastError();

			if (errorCode == CRSTL_ERROR_ALREADY_EXISTS)
			{
				return filesystem_return::success;
			}
			else
			{
				return filesystem_return::error;
			}
		}
	}

	namespace detail
	{
		inline path compute_temp_path()
		{
			const uint32_t MaxPathSize = 260 + 1; // MAX_PATH

			CHAR tempPath[MaxPathSize];

			DWORD shortTempPathLength = GetTempPathA(MaxPathSize, tempPath);
			if (shortTempPathLength > 0)
			{
				// From the docs: "You can use the same buffer you used for the lpszShortPath parameter"
				DWORD longTempPathLength = GetLongPathNameA(tempPath, tempPath, MaxPathSize);
				if (longTempPathLength > 0)
				{
					return path(tempPath);
				}
			}

			return path();
		}
	}
};
