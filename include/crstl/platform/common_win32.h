#pragma once

crstl_warning_unscoped_enum_begin

#define CRSTL_INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

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
	inline void close_handle_safe(void*& handle)
	{
		if (handle != CRSTL_INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
			handle = CRSTL_INVALID_HANDLE_VALUE;
		}
	}
};

crstl_warning_unscoped_enum_end