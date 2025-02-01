#pragma once

#include "common_win32.h"

// https://learn.microsoft.com/en-us/windows/win32/sync/critical-section-objects
extern "C"
{
	typedef struct _RTL_CRITICAL_SECTION RTL_CRITICAL_SECTION;
	typedef struct _RTL_CRITICAL_SECTION* PRTL_CRITICAL_SECTION;
	typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
	typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
	typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;
	
	__declspec(dllimport) void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	__declspec(dllimport) void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	
	__declspec(dllimport) void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	__declspec(dllimport) BOOL TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	__declspec(dllimport) void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
};

crstl_module_export namespace crstl
{
	namespace detail
	{
		typedef struct _LIST_ENTRY {
			struct _LIST_ENTRY* Flink;
			struct _LIST_ENTRY* Blink;
		} LIST_ENTRY, *PLIST_ENTRY;
	
		typedef struct _RTL_CRITICAL_SECTION_DEBUG {
			WORD Type;
			WORD CreatorBackTraceIndex;
			struct _RTL_CRITICAL_SECTION* CriticalSection;
			LIST_ENTRY ProcessLocksList;
			DWORD EntryCount;
			DWORD ContentionCount;
			DWORD Flags;
			WORD CreatorBackTraceIndexHigh;
			WORD Identifier;
		} RTL_CRITICAL_SECTION_DEBUG, * PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, * PRTL_RESOURCE_DEBUG;
	
		#pragma pack(push, 8)
	
		typedef struct _RTL_CRITICAL_SECTION {
			PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
	
			//
			//  The following three fields control entering and exiting the critical
			//  section for the resource
			//
	
			LONG LockCount;
			LONG RecursionCount;
			HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
			HANDLE LockSemaphore;
			ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
		} RTL_CRITICAL_SECTION, * PRTL_CRITICAL_SECTION;
	
		#pragma pack(pop)
	
		typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
		typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
		typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;
	}

	class critical_section final : public critical_section_base
	{
	public:

		critical_section()
		{
			InitializeCriticalSection((LPCRITICAL_SECTION)&m_critical_section);
		}

		void lock()
		{
			EnterCriticalSection((LPCRITICAL_SECTION)&m_critical_section);
		}

		crstl_nodiscard bool try_lock()
		{
			return TryEnterCriticalSection((LPCRITICAL_SECTION)&m_critical_section) != 0;
		}

		void unlock()
		{
			LeaveCriticalSection((LPCRITICAL_SECTION)&m_critical_section);
		}

		~critical_section()
		{
			DeleteCriticalSection((LPCRITICAL_SECTION)&m_critical_section);
		}

	protected:

		detail::CRITICAL_SECTION m_critical_section;
	};
};