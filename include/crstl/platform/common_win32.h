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

enum _GET_FILEEX_INFO_LEVELS;
typedef _GET_FILEEX_INFO_LEVELS GET_FILEEX_INFO_LEVELS;

namespace crstl
{
	typedef struct _filetime
	{
		DWORD dwLowDateTime;
		DWORD dwHighDateTime;
	} filetime;

	crstl_warning_anonymous_struct_union_begin
	union large_integer
	{
		struct
		{
			DWORD low_part;
			LONG high_part;
		};

		LONGLONG quad_part;
	};
	crstl_warning_anonymous_struct_union_end

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
	}
};

crstl_warning_unscoped_enum_end