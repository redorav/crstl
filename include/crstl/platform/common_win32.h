#pragma once

crstl_warning_unscoped_enum_begin

// Handleapi.h
#define CRSTL_INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

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

#define CRSTL_FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001   
#define CRSTL_FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002   
#define CRSTL_FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004   
#define CRSTL_FILE_NOTIFY_CHANGE_SIZE         0x00000008   
#define CRSTL_FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010   
#define CRSTL_FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020   
#define CRSTL_FILE_NOTIFY_CHANGE_CREATION     0x00000040   
#define CRSTL_FILE_NOTIFY_CHANGE_SECURITY     0x00000100   

#define CRSTL_FILE_ACTION_ADDED                   0x00000001   
#define CRSTL_FILE_ACTION_REMOVED                 0x00000002   
#define CRSTL_FILE_ACTION_MODIFIED                0x00000003   
#define CRSTL_FILE_ACTION_RENAMED_OLD_NAME        0x00000004   
#define CRSTL_FILE_ACTION_RENAMED_NEW_NAME        0x00000005   

#define CRSTL_FILE_READ_DATA            ( 0x0001 )    // file & pipe
#define CRSTL_FILE_LIST_DIRECTORY       ( 0x0001 )    // directory

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

#define CRSTL_FILE_FLAG_WRITE_THROUGH         0x80000000
#define CRSTL_FILE_FLAG_OVERLAPPED            0x40000000
#define CRSTL_FILE_FLAG_NO_BUFFERING          0x20000000
#define CRSTL_FILE_FLAG_RANDOM_ACCESS         0x10000000
#define CRSTL_FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define CRSTL_FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define CRSTL_FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define CRSTL_FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define CRSTL_FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define CRSTL_FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define CRSTL_FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

// Winerror.h
#define CRSTL_ERROR_FILE_NOT_FOUND             2L
#define CRSTL_ERROR_ACCESS_DENIED              5L
#define CRSTL_ERROR_SHARING_VIOLATION          32L
#define CRSTL_ERROR_ALREADY_EXISTS             183L

// WinNIs.h
#define CRSTL_CP_UTF8 65001

typedef char                CHAR;
typedef signed char         INT8;
typedef unsigned char       UCHAR;
typedef unsigned char       UINT8;
typedef unsigned char       BYTE;
typedef short               SHORT;
typedef signed short        INT16;
typedef unsigned short      USHORT;
typedef unsigned short      UINT16;
typedef unsigned short      WORD;
typedef int                 INT;
typedef signed int          INT32;
typedef unsigned int        UINT;
typedef unsigned int        UINT32;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef __int64             LONGLONG;
typedef __int64             LONG64;
typedef signed __int64      INT64;
typedef unsigned __int64    ULONGLONG;
typedef unsigned __int64    DWORDLONG;
typedef unsigned __int64    ULONG64;
typedef unsigned __int64    DWORD64;
typedef unsigned __int64    UINT64;

typedef BYTE* LPBYTE;
typedef DWORD* LPDWORD;

typedef int BOOL;
typedef BOOL* LPBOOL;

typedef const CHAR* LPCCH, * PCCH;
typedef CHAR* LPSTR;
typedef const CHAR* LPCSTR;

typedef wchar_t WCHAR;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR, *PCWSTR;

typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef const WCHAR *LPCWCH, *PCWCH;

typedef unsigned __int64 ULONG_PTR, * PULONG_PTR;

typedef void* PVOID;
typedef void* LPVOID;
typedef const void* LPCVOID;

typedef LONG HRESULT;
typedef PVOID HANDLE;
typedef HANDLE* PHANDLE;

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;
typedef HINSTANCE HMODULE;

typedef __int64          INT_PTR;
typedef unsigned __int64 UINT_PTR;
typedef __int64          LONG_PTR;
typedef unsigned __int64 ULONG_PTR;

typedef SHORT* PSHORT;
typedef LONG* PLONG;

#if defined(_WIN64)
typedef INT_PTR(*FARPROC)();
typedef INT_PTR(*NEARPROC)();
typedef INT_PTR(*PROC)();
#else
typedef int (*FARPROC)();
typedef int (*NEARPROC)();
typedef int (*PROC)();
#endif

typedef union _LARGE_INTEGER LARGE_INTEGER;
typedef LARGE_INTEGER* PLARGE_INTEGER;
struct _SECURITY_ATTRIBUTES;
typedef _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
typedef _SECURITY_ATTRIBUTES* LPSECURITY_ATTRIBUTES;
typedef struct _STARTUPINFOW STARTUPINFOW;
typedef struct _PROCESS_INFORMATION PROCESS_INFORMATION;
typedef struct _OVERLAPPED* LPOVERLAPPED;

typedef struct _WIN32_FIND_DATAA WIN32_FIND_DATAA;
typedef struct _WIN32_FIND_DATAA* LPWIN32_FIND_DATAA;

typedef struct _WIN32_FIND_DATAW WIN32_FIND_DATAW;
typedef struct _WIN32_FIND_DATAW* LPWIN32_FIND_DATAW;

typedef struct _FILE_NOTIFY_INFORMATION FILE_NOTIFY_INFORMATION;

typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;

enum _GET_FILEEX_INFO_LEVELS;
typedef _GET_FILEEX_INFO_LEVELS GET_FILEEX_INFO_LEVELS;

typedef void (*LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
typedef unsigned long(*PTHREAD_START_ROUTINE)(void* lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

//void PrintLastWindowsError()
//{
//	DWORD error = GetLastError();
//
//	LPSTR messageBuffer;
//	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US), (LPSTR)&messageBuffer, 0, NULL);
//
//	OutputDebugStringA(messageBuffer);
//
//	LocalFree(messageBuffer);
//}

namespace crstl
{
	namespace detail
	{
		crstl_warning_anonymous_struct_union_begin
		typedef struct _filetime
		{
			DWORD dwLowDateTime;
			DWORD dwHighDateTime;
		} filetime;

		union large_integer
		{
			struct
			{
				DWORD low_part;
				LONG high_part;
			};

			LONGLONG quad_part;
		};

		typedef struct _win32_file_attribute_data
		{
			DWORD dwFileAttributes;
			filetime ftCreationTime;
			filetime ftLastAccessTime;
			filetime ftLastWriteTime;
			DWORD nFileSizeHigh;
			DWORD nFileSizeLow;
		} win32_file_attribute_data, *lpwin32_file_attribute_data;

		typedef enum _get_fileex_info_levels
		{
			GetFileExInfoStandard,
			GetFileExMaxInfoLevel
		} get_fileex_info_levels;

		typedef struct _win32_find_dataa
		{
			DWORD dwFileAttributes;
			filetime ftCreationTime;
			filetime ftLastAccessTime;
			filetime ftLastWriteTime;
			DWORD nFileSizeHigh;
			DWORD nFileSizeLow;
			DWORD dwReserved0;
			DWORD dwReserved1;
			CHAR cFileName[260];
			CHAR cAlternateFileName[14];
		} win32_find_dataa;

		typedef struct _win32_find_dataw
		{
			DWORD dwFileAttributes;
			filetime ftCreationTime;
			filetime ftLastAccessTime;
			filetime ftLastWriteTime;
			DWORD nFileSizeHigh;
			DWORD nFileSizeLow;
			DWORD dwReserved0;
			DWORD dwReserved1;
			WCHAR cFileName[260];
			WCHAR cAlternateFileName[14];
		} win32_find_dataw;

		typedef struct _file_notify_information
		{
			DWORD NextEntryOffset;
			DWORD Action;
			DWORD FileNameLength;
			WCHAR FileName[1];
		} file_notify_information, *pfile_notify_information;

		typedef struct _overlapped
		{
			ULONG_PTR Internal;
			ULONG_PTR InternalHigh;
			union
			{
				struct
				{
					DWORD Offset;
					DWORD OffsetHigh;
				};
				PVOID Pointer;
			};

			HANDLE hEvent;
		} overlapped, *lpoverlapped;

		crstl_warning_anonymous_struct_union_end
	}
};

extern "C"
{
	__declspec(dllimport) int MultiByteToWideChar
	(
		UINT CodePage,
		DWORD dwFlags,
		LPCCH lpMultiByteStr,
		int cbMultiByte,
		LPWSTR lpWideCharStr,
		int cchWideChar
	);

	__declspec(dllimport) int WideCharToMultiByte
	(
		UINT CodePage,
		DWORD dwFlags,
		LPCWCH lpWideCharStr,
		int cchWideChar,
		LPSTR lpMultiByteStr,
		int cbMultiByte,
		LPCCH lpDefaultChar,
		LPBOOL lpUsedDefaultChar
	);

	__declspec(dllimport) BOOL CloseHandle(HANDLE hObject);

	__declspec(dllimport) DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

	__declspec(dllimport) DWORD GetLastError(void);

	__declspec(dllimport) UINT GetOEMCP(void);

	// Events

	__declspec(dllimport) HANDLE CreateEventA(SECURITY_ATTRIBUTES* lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);

	__declspec(dllimport) BOOL ResetEvent(HANDLE hEvent);

	__declspec(dllimport) DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

	__declspec(dllimport) DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);

	// Files

	__declspec(dllimport) HANDLE CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

	__declspec(dllimport) HANDLE CreateFileW(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

	__declspec(dllimport) BOOL DeleteFileA(LPCSTR lpFileName);

	__declspec(dllimport) BOOL DeleteFileW(LPCWSTR lpFileName);

	__declspec(dllimport) BOOL CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);

	__declspec(dllimport) BOOL CopyFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists);

	__declspec(dllimport) BOOL MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);

	__declspec(dllimport) BOOL MoveFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName);

	__declspec(dllimport) BOOL ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);

	__declspec(dllimport) BOOL WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);

	__declspec(dllimport) DWORD SetFilePointer(HANDLE, LONG, PLONG, DWORD);

	__declspec(dllimport) BOOL GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize);

	__declspec(dllimport) BOOL GetFileAttributesExA(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation);

	__declspec(dllimport) BOOL GetFileAttributesExW(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation);

	// Directories

	__declspec(dllimport) BOOL CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

	__declspec(dllimport) BOOL CreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

	// Paths

	__declspec(dllimport) DWORD GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer);

	__declspec(dllimport) DWORD GetTempPathW(DWORD nBufferLength, LPWSTR lpBuffer);

	__declspec(dllimport) DWORD GetLongPathNameA(LPCSTR lpszShortPath, LPSTR lpszLongPath, DWORD cchBuffer);

	__declspec(dllimport) DWORD GetLongPathNameW(LPCWSTR lpszShortPath, LPWSTR lpszLongPath, DWORD cchBuffer);

	__declspec(dllimport) HANDLE FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);

	__declspec(dllimport) BOOL FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

	__declspec(dllimport) HANDLE FindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);

	__declspec(dllimport) BOOL FindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);

	__declspec(dllimport) BOOL FindClose(HANDLE hFindFile);

	__declspec(dllimport) BOOL ReadDirectoryChangesW
	(
		HANDLE hDirectory,
		LPVOID lpBuffer,
		DWORD nBufferLength,
		BOOL bWatchSubtree,
		DWORD dwNotifyFilter,
		LPDWORD lpBytesReturned,
		LPOVERLAPPED lpOverlapped,
		LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);

	// Module filename

	__declspec(dllimport) DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

	__declspec(dllimport) DWORD GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);

	__declspec(dllimport) DWORD GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);

	__declspec(dllimport) DWORD GetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer);

	__declspec(dllimport) BOOL SetCurrentDirectoryA(LPCSTR lpPathName);

	__declspec(dllimport) BOOL SetCurrentDirectoryW(LPCWSTR lpPathName);
};

namespace crstl
{
	namespace detail
	{
		template<typename Dummy>
		struct win32_filesystem_globals
		{
			static const UINT win32_codepage;
		};

		template<typename Dummy>
		const UINT win32_filesystem_globals<Dummy>::win32_codepage = GetOEMCP();

		// Return whether Windows has the UTF-8 codepage enabled system-wide
		// Mainly used to avoid string conversions when Windows supports the behavior natively
		inline bool win32_is_utf8()
		{
			return win32_filesystem_globals<void>::win32_codepage == CRSTL_CP_UTF8;
		}

		inline void close_handle_safe(void*& handle)
		{
			if (handle != CRSTL_INVALID_HANDLE_VALUE)
			{
				CloseHandle(handle);
				handle = CRSTL_INVALID_HANDLE_VALUE;
			}
		}

		inline int win32_utf8_to_utf16(const CHAR* path, size_t path_length, WCHAR* destination_path, size_t destination_path_length)
		{
			int end_position = MultiByteToWideChar(CRSTL_CP_UTF8, 0, path, (int)path_length, destination_path, (int)destination_path_length);
			destination_path[end_position] = L'\0';
			return end_position;
		}

		inline int win32_utf16_to_utf8(const WCHAR* path, size_t path_length, CHAR* destination_path, size_t destination_path_length)
		{
			int end_position = WideCharToMultiByte(CRSTL_CP_UTF8, 0, path, (int)path_length, destination_path, (int)destination_path_length, nullptr, nullptr);
			destination_path[end_position] = '\0';
			return end_position;
		}
	}
};

crstl_warning_unscoped_enum_end