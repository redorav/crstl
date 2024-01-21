#pragma once

#define CRSTL_INVALID_HANDLE_VALUE ((crstl::HANDLE)(crstl::LONG_PTR)-1)

namespace crstl
{
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

	typedef const CHAR* LPCCH, *PCCH;
	typedef CHAR* LPSTR;
	typedef const CHAR* LPCSTR;

	typedef wchar_t WCHAR;
	typedef WCHAR* LPWSTR;
	typedef const WCHAR* LPCWSTR, *PCWSTR;

	typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

	typedef LONG HRESULT;
	typedef void* HANDLE;
	typedef void* HINSTANCE;
	typedef void* HMODULE;

	typedef HANDLE* PHANDLE;

	typedef void* PVOID;
	typedef void* LPVOID;

	typedef __int64             INT_PTR;
	typedef unsigned __int64    UINT_PTR;
	typedef __int64             LONG_PTR;
	typedef unsigned __int64    ULONG_PTR;

#if defined(_WIN64)
	typedef INT_PTR(*FARPROC)();
	typedef INT_PTR(*NEARPROC)();
	typedef INT_PTR(*PROC)();
#else
	typedef int (*FARPROC)();
	typedef int (*NEARPROC)();
	typedef int (*PROC)();
#endif
};

extern "C"
{
	__declspec(dllimport) int MultiByteToWideChar
	(
		crstl::UINT CodePage,
		crstl::DWORD dwFlags,
		crstl::LPCCH lpMultiByteStr,
		int cbMultiByte,
		crstl::LPWSTR lpWideCharStr,
		int cchWideChar
	);

	__declspec(dllimport) crstl::BOOL CloseHandle(crstl::HANDLE hObject);

	__declspec(dllimport) crstl::DWORD WaitForSingleObject(crstl::HANDLE hHandle, crstl::DWORD dwMilliseconds);
};

namespace crstl
{
	inline void close_handle_safe(void*& handle)
	{
		if (handle)
		{
			CloseHandle(handle);
			handle = nullptr;
		}
	}
};