#pragma once

#include "common_win32.h"

#include "sdkddkver.h"

#include "crstl/utility/string_length.h"

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

	__declspec(dllimport) DWORD ResumeThread(HANDLE hThread);

	__declspec(dllimport) __declspec(noreturn) void ExitThread(DWORD dwExitCode);

	__declspec(dllimport) BOOL SetThreadPriority(HANDLE hThread, int nPriority);

	__declspec(dllimport) void Sleep(DWORD dwMilliseconds);

	__declspec(dllimport) DWORD GetCurrentThreadId();

	__declspec(dllimport) HANDLE GetCurrentThread();

	__declspec(dllimport) HRESULT SetThreadDescription(HANDLE hThread, const wchar_t* lpThreadDescription);

	__declspec(dllimport) void RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments, const ULONG_PTR* lpArguments);

	unsigned long _exception_code(void);

	__declspec(dllimport) FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

	__declspec(dllimport) HMODULE GetModuleHandleA(LPCSTR lpModuleName);
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

crstl_module_export namespace crstl
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
			#if WINVER >= _WIN32_WINNT_WIN10

			HRESULT(*SetThreadDescriptionFunction)(HANDLE, PCWSTR) =
				reinterpret_cast<HRESULT(*)(HANDLE, PCWSTR)>(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "SetThreadDescription"));

			if (SetThreadDescriptionFunction)
			{
				static const size_t kMaxDescriptionLength = 255;
				wchar_t wdebug_name[kMaxDescriptionLength];
				int dw_length = MultiByteToWideChar
				(
					0 /*CP_ACP*/, 0,
					thread_data->parameters.debug_name, (int)crstl::string_length(thread_data->parameters.debug_name),
					wdebug_name, kMaxDescriptionLength
				);

				wdebug_name[dw_length] = L'\0';

				HRESULT hResult = SetThreadDescriptionFunction(GetCurrentThread(), wdebug_name); crstl_unused(hResult);
			}

			#endif
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

		thread() crstl_noexcept : m_thread_handle(CRSTL_INVALID_HANDLE_VALUE), m_finished_event(CRSTL_INVALID_HANDLE_VALUE), m_thread_id(0) {}

		thread(thread&& other) crstl_noexcept
		{
			m_thread_handle = other.m_thread_handle;
			m_finished_event = other.m_finished_event;
			m_thread_id = other.m_thread_id;
			other.m_thread_handle = CRSTL_INVALID_HANDLE_VALUE;
			other.m_finished_event = CRSTL_INVALID_HANDLE_VALUE;
			other.m_thread_id = 0;
		}

		thread& operator = (thread&& other) crstl_noexcept
		{
			join();

			m_thread_handle = other.m_thread_handle;
			m_thread_id = other.m_thread_id;
			m_finished_event = other.m_finished_event;
			other.m_thread_handle = CRSTL_INVALID_HANDLE_VALUE;
			other.m_finished_event = CRSTL_INVALID_HANDLE_VALUE;
			other.m_thread_id = 0;

			return *this;
		}

		template<typename Function, typename ... Args>
		explicit thread(const thread_parameters& parameters, Function&& func, Args&& ... args) crstl_noexcept
		{
			crstl::thread_data<Function, Args...>* thread_data = new crstl::thread_data<Function, Args...>
			{
				this,
				parameters,
				func,
				crstl::tuple<Args...>(crstl_forward(Args, args)...)
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
			if (m_thread_handle != CRSTL_INVALID_HANDLE_VALUE)
			{
				WaitForSingleObject(m_thread_handle, 0xffffffff);
				CloseHandle(m_thread_handle);
				m_thread_handle = CRSTL_INVALID_HANDLE_VALUE;
			}
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
					default:
						result = 0;
				}

				if (result)
				{
					m_priority = priority;
				}
			}
		}

		~thread()
		{
			join();
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