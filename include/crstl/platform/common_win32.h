#pragma once

namespace crstl
{
	typedef unsigned int UINT;
	typedef unsigned long DWORD;
	typedef long LONG;
	typedef int BOOL;
	typedef char CHAR;

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