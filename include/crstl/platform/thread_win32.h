#pragma once

#include "common_win32.h"

typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
typedef unsigned long(*PTHREAD_START_ROUTINE)(void* lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

extern "C"
{
	__declspec(dllimport) void* CreateThread
	(
		SECURITY_ATTRIBUTES* lpThreadAttributes,
		size_t dwStackSize,
		LPTHREAD_START_ROUTINE lpStartAddress,
		void* lpParameter,
		unsigned long dwCreationFlags,
		unsigned long* lpThreadId
	);

	__declspec(dllimport) crstl::DWORD ResumeThread(crstl::HANDLE hThread);

	__declspec(dllimport) __declspec(noreturn) void ExitThread(crstl::DWORD dwExitCode);

	__declspec(dllimport) crstl::BOOL SetThreadPriority(crstl::HANDLE hThread, int nPriority);

	__declspec(dllimport) void Sleep(crstl::DWORD dwMilliseconds);

	__declspec(dllimport) crstl::DWORD GetCurrentThreadId();

	__declspec(dllimport) crstl::HANDLE GetCurrentThread();

	__declspec(dllimport) crstl::HRESULT SetThreadDescription(crstl::HANDLE hThread, const wchar_t* lpThreadDescription);

	__declspec(dllimport) crstl::HANDLE CreateEventA(SECURITY_ATTRIBUTES* lpEventAttributes, crstl::BOOL bManualReset, crstl::BOOL bInitialState, crstl::LPCSTR lpName);

	__declspec(dllimport) crstl::DWORD WaitForSingleObject(crstl::HANDLE hHandle, crstl::DWORD dwMilliseconds);

	__declspec(dllimport) crstl::BOOL CloseHandle(crstl::HANDLE hObject);

	__declspec(dllimport) void RaiseException(crstl::DWORD dwExceptionCode, crstl::DWORD dwExceptionFlags, crstl::DWORD nNumberOfArguments, const crstl::ULONG_PTR* lpArguments);

	unsigned long _exception_code(void);
};

#define CRSTL_CREATE_SUSPENDED 0x00000004

#define CRSTL_THREAD_BASE_PRIORITY_LOWRT  15    // value that gets a thread to LowRealtime-1
#define CRSTL_THREAD_BASE_PRIORITY_MAX    2     // maximum thread base priority boost
#define CRSTL_THREAD_BASE_PRIORITY_MIN    (-2)  // minimum thread base priority boost
#define CRSTL_THREAD_BASE_PRIORITY_IDLE   (-15) // value that gets a thread to idle

#define CRSTL_THREAD_PRIORITY_LOWEST          CRSTL_THREAD_BASE_PRIORITY_MIN
#define CRSTL_THREAD_PRIORITY_BELOW_NORMAL    (CRSTL_THREAD_PRIORITY_LOWEST + 1)
#define CRSTL_THREAD_PRIORITY_NORMAL          0
#define CRSTL_THREAD_PRIORITY_HIGHEST         CRSTL_THREAD_BASE_PRIORITY_MAX
#define CRSTL_THREAD_PRIORITY_ABOVE_NORMAL    (CRSTL_THREAD_PRIORITY_HIGHEST - 1)
#define CRSTL_THREAD_PRIORITY_ERROR_RETURN    (0x7fffffff) // MAXLONG

#define CRSTL_THREAD_PRIORITY_TIME_CRITICAL   CRSTL_THREAD_BASE_PRIORITY_LOWRT
#define CRSTL_THREAD_PRIORITY_IDLE            CRSTL_THREAD_BASE_PRIORITY_IDLE

#define CRSTL_MS_VC_EXCEPTION 0x406D1388

#define CRSTL_EXCEPTION_EXECUTE_HANDLER      1
#define CRSTL_EXCEPTION_CONTINUE_SEARCH      0
#define CRSTL_EXCEPTION_CONTINUE_EXECUTION (-1)

namespace crstl
{
	#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO {
		DWORD dwType;     // Must be 0x1000
		char* szName;     // Pointer to name (in user address space)
		DWORD dwThreadID; // Thread ID (-1 for caller thread)
		DWORD dwFlags;    // Reserved for future use; must be zero
	} THREADNAME_INFO;
	#pragma pack(pop)

	template<typename Function, typename ... Args>
	inline unsigned long MainThreadFn(void* parameter)
	{
		crstl::thread_data<Function, Args...>* thread_data = (crstl::thread_data<Function, Args...>*)parameter;

		if (thread_data->parameters.debug_name)
		{
			// Set name of thread here
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = (char*)thread_data->parameters.debug_name;
			info.dwThreadID = GetCurrentThreadId();
			info.dwFlags = 0;

			__try
			{
				RaiseException(CRSTL_MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(DWORD), (const ULONG_PTR*)(&info));
			}
			__except (_exception_code() == CRSTL_MS_VC_EXCEPTION ? CRSTL_EXCEPTION_CONTINUE_EXECUTION : CRSTL_EXCEPTION_EXECUTE_HANDLER)
			{
			}

			// https://learn.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2022
			HRESULT hResult = SetThreadDescription(GetCurrentThread(), L"ThisIsMyThreadName!"); (void)hResult;
		}

		// TODO Do we need the result of the function
		call(thread_data->function, thread_data->function_args, crstl::make_index_sequence<sizeof...(Args)>());

		delete thread_data;

		ExitThread(0);

		// ExitThread takes care of it
		// return 0;
	}

	class thread final : thread_base
	{
	public:

		thread() crstl_nodiscard : m_thread_handle((HANDLE)-1), m_thread_id(0) {}

		thread(thread&& other) crstl_nodiscard
		{
			m_thread_handle = other.m_thread_handle;
			m_thread_id = other.m_thread_id;
			other.m_thread_handle = (HANDLE)-1;
			other.m_thread_id = 0;
		}

		template<typename Function, typename ... Args>
		explicit thread(const thread_parameters& parameters, Function&& func, Args&& ... args) crstl_noexcept
		{
			crstl::thread_data<Function, Args...>* thread_data = new crstl::thread_data<Function, Args...>
			{
				this,
				parameters,
				func,
				crstl::tuple<Args...>(crstl::forward<Args>(args)...)
			};

			unsigned long thread_id = 0;

			m_thread_handle = CreateThread
			(
				nullptr,
				parameters.stack_size ? parameters.stack_size : DefaultStackSize,
				MainThreadFn<Function, Args...>,
				(void*)thread_data,
				parameters.delay_start ? CRSTL_CREATE_SUSPENDED : 0,
				&thread_id
			);

			m_started = !parameters.delay_start;

			m_thread_id = thread_id;

			set_priority(parameters.priority);

			bool manual_reset = true;
			m_finished_event = CreateEventA(nullptr, manual_reset ? true : false, false, nullptr);
		}

		void start()
		{
			if (!m_started)
			{
				ResumeThread(m_thread_handle);
				m_started = true;
			}
		}

		void join()
		{
			if (m_thread_handle)
			{
				WaitForSingleObject(m_thread_handle, 0xffffffff);
			}

			CloseHandle(m_thread_handle);
			m_thread_handle = (HANDLE)-1;
		}

		void set_priority(thread_priority::t priority)
		{
			if (m_priority != priority)
			{
				BOOL result = 0;

				switch (priority)
				{
				case thread_priority::idle:
					result = SetThreadPriority(m_thread_handle, CRSTL_THREAD_PRIORITY_IDLE);
					break;
				case thread_priority::lowest:
					result = SetThreadPriority(m_thread_handle, CRSTL_THREAD_PRIORITY_LOWEST);
					break;
				case thread_priority::below_normal:
					result = SetThreadPriority(m_thread_handle, CRSTL_THREAD_PRIORITY_BELOW_NORMAL);
					break;
				case thread_priority::normal:
					result = SetThreadPriority(m_thread_handle, CRSTL_THREAD_PRIORITY_NORMAL);
					break;
				case thread_priority::abovenormal:
					result = SetThreadPriority(m_thread_handle, CRSTL_THREAD_PRIORITY_ABOVE_NORMAL);
					break;
				case thread_priority::highest:
					result = SetThreadPriority(m_thread_handle, CRSTL_THREAD_PRIORITY_HIGHEST);
					break;
				default: result = 0;
				}

				if (result)
				{
					m_priority = priority;
				}
			}
		}

		~thread()
		{
			if (m_thread_handle != (HANDLE)-1)
			{
				join();
			}
		}

	private:

		HANDLE m_thread_handle;

		HANDLE m_finished_event;

		uint64_t m_thread_id;
	};

	namespace this_thread
	{
		// Yields timeslice back to the OS
		inline void yield()
		{
			// _Thrd_yield();    // Maps to Sleep(0) according to Wine
			// SwitchToThread(); // Only yields to threads in current processor
			// YieldProcessor(); // Macro that maps to _mm_pause, which does not release the CPU for the scheduler
			Sleep(0);
		}

		inline void sleep_for(uint32_t milliseconds)
		{
			Sleep(milliseconds);
		}
	};
};