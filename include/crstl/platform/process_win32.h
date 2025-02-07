#pragma once

#include "crstl/process.h"

#include "common_win32.h"

#include "crstl/utility/string_length.h"

#include "crstl/utility/memory_ops.h"

#define CRSTL_WAIT_ABANDONED 0x00000080L
#define CRSTL_WAIT_OBJECT_0  0x00000000L
#define CRSTL_WAIT_TIMEOUT   0x00000102L
#define CRSTL_WAIT_FAILED    0xFFFFFFFF

#define CRSTL_STATUS_PENDING ((DWORD   )0x00000103L)

extern "C"
{
	__declspec(dllimport) BOOL CreatePipe
	(
		PHANDLE hReadPipe,
		PHANDLE hWritePipe,
		LPSECURITY_ATTRIBUTES lpPipeAttributes,
		DWORD nSize
	);

	__declspec(dllimport) BOOL CreateProcessW
	(
		LPCWSTR lpApplicationName,
		LPWSTR lpCommandLine,
		SECURITY_ATTRIBUTES* lpProcessAttributes,
		SECURITY_ATTRIBUTES* lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCWSTR lpCurrentDirectory,
		STARTUPINFOW* lpStartupInfo,
		PROCESS_INFORMATION* lpProcessInformation
	);

	__declspec(dllimport) BOOL SetHandleInformation(HANDLE hObject, DWORD dwMask, DWORD dwFlags);

	__declspec(dllimport) BOOL GetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode);

	__declspec(dllimport) BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

	__declspec(dllimport) BOOL TerminateProcess(HANDLE hProcess, UINT uExitCode);
};

#define CRSTL_CREATE_NO_WINDOW 0x08000000

#define CRSTL_HANDLE_FLAG_INHERIT             0x00000001
#define CRSTL_HANDLE_FLAG_PROTECT_FROM_CLOSE  0x00000002

#define CRSTL_STARTF_USESTDHANDLES    0x00000100

crstl_module_export namespace crstl
{
	namespace detail
	{
		typedef struct _STARTUPINFOW
		{
			DWORD   cb;
			LPWSTR  lpReserved;
			LPWSTR  lpDesktop;
			LPWSTR  lpTitle;
			DWORD   dwX;
			DWORD   dwY;
			DWORD   dwXSize;
			DWORD   dwYSize;
			DWORD   dwXCountChars;
			DWORD   dwYCountChars;
			DWORD   dwFillAttribute;
			DWORD   dwFlags;
			WORD    wShowWindow;
			WORD    cbReserved2;
			LPBYTE  lpReserved2;
			HANDLE  hStdInput;
			HANDLE  hStdOutput;
			HANDLE  hStdError;
		} STARTUPINFOW, * LPSTARTUPINFOW;

		typedef struct _PROCESS_INFORMATION
		{
			HANDLE hProcess;
			HANDLE hThread;
			DWORD dwProcessId;
			DWORD dwThreadId;
		} PROCESS_INFORMATION, * PPROCESS_INFORMATION, * LPPROCESS_INFORMATION;

		typedef struct _SECURITY_ATTRIBUTES {
			DWORD nLength;
			LPVOID lpSecurityDescriptor;
			BOOL bInheritHandle;
		} SECURITY_ATTRIBUTES, * PSECURITY_ATTRIBUTES, * LPSECURITY_ATTRIBUTES;
	};

	class process : public process_base
	{
	public:

		process() 
			: process_base()
			, m_process_handle(CRSTL_INVALID_HANDLE_VALUE)
			, m_stdout_read_handle(CRSTL_INVALID_HANDLE_VALUE)
		{}

		process(const char* executable, const char* args) : process()
		{
			crstl_assert(executable != nullptr);

			detail::STARTUPINFOW startup_info = {};
			startup_info.cb = sizeof(startup_info);

			HANDLE stdout_read = nullptr;
			HANDLE stdout_write = nullptr;

			detail::SECURITY_ATTRIBUTES security_attributes = {};
			security_attributes.nLength = sizeof(detail::SECURITY_ATTRIBUTES);
			security_attributes.bInheritHandle = true;
			security_attributes.lpSecurityDescriptor = nullptr;

			// Pipe process's std out to a handle we can read later
			if (CreatePipe(&stdout_read, &stdout_write, (SECURITY_ATTRIBUTES*)&security_attributes, 0))
			{
				if (SetHandleInformation(stdout_read, CRSTL_HANDLE_FLAG_INHERIT, 0))
				{
					// Redirect both std out and std error to the same location
					startup_info.hStdError = stdout_write;
					startup_info.hStdOutput = stdout_write;
					startup_info.dwFlags |= CRSTL_STARTF_USESTDHANDLES;
				}
			}

			// Convert to UTF-16 to be able to handle complex characters
			bool has_args = args != nullptr && args[0] != '\0';
			
			size_t executable_length = crstl::string_length(executable);
			size_t total_length = executable_length + 1; // Include the null terminator

			size_t args_length = 0;
			if (has_args)
			{
				args_length = crstl::string_length(args);
				total_length += 1; // Space between executable and args
				total_length += args_length;
			}

			// Allocate full command line on stack
			wchar_t* commandline_w = (wchar_t*)crstl_alloca(total_length * sizeof(wchar_t));
			int end_position = MultiByteToWideChar(0 /*CP_ACP*/, 0, executable, (int)executable_length, commandline_w, (int)total_length);

			// If we have some args, append them to the full command line
			if (has_args)
			{
				commandline_w[end_position++] = ' ';
				end_position += MultiByteToWideChar(0 /*CP_ACP*/, 0, args, (int)args_length, commandline_w + end_position, (int)(total_length - end_position));
			}

			// Null terminate full command line
			commandline_w[end_position] = L'\0';

			detail::PROCESS_INFORMATION process_info = {};

			bool createprocess_result = CreateProcessW
			(
				nullptr,
				&commandline_w[0],
				nullptr,                // Process handle not inheritable
				nullptr,                // Thread handle not inheritable
				1,                      // Handles are inherited
				CRSTL_CREATE_NO_WINDOW, // Don't create a window
				nullptr,                // Use parent's environment block
				nullptr,                // Use parent's starting directory 
				(STARTUPINFOW*)&startup_info,
				(PROCESS_INFORMATION*)&process_info
			);

			if (createprocess_result)
			{
				// We don't need the primary thread of the process
				detail::close_handle_safe(process_info.hThread);
				detail::close_handle_safe(startup_info.hStdOutput);

				m_state = process_state::launched;
				m_process_handle = process_info.hProcess;
				m_stdout_read_handle = stdout_read;
			}
			else
			{
				m_state = process_state::error_failed_to_launch;
			}
		}

		~process()
		{
			detail::close_handle_safe(m_process_handle);
			detail::close_handle_safe(m_stdout_read_handle);
		}

		process(process&& other) crstl_noexcept
		{
			m_process_handle = other.m_process_handle;
			m_stdout_read_handle = other.m_stdout_read_handle;
			m_state = other.m_state;

			other.m_process_handle = nullptr;
			other.m_stdout_read_handle = nullptr;
			other.m_state = process_state::undefined;
		}

		process& operator = (process&& other) crstl_noexcept
		{
			terminate();

			m_process_handle = other.m_process_handle;
			m_stdout_read_handle = other.m_stdout_read_handle;
			m_state = other.m_state;

			other.m_process_handle = nullptr;
			other.m_stdout_read_handle = nullptr;
			other.m_state = process_state::undefined;

			return *this;
		}

		bool is_alive()
		{
			if (m_state == process_state::launched)
			{
				DWORD wait_result = WaitForSingleObject(m_process_handle, 0);
				return wait_result != CRSTL_WAIT_OBJECT_0;
			}
			else
			{
				return false;
			}
		}

		process_exit_code wait()
		{
			if (m_state == process_state::launched)
			{
				// If wait timeout is the maximum, we wait until it finishes
				DWORD wait_result = WaitForSingleObject(m_process_handle, 0xffffffff);

				if (wait_result == CRSTL_WAIT_OBJECT_0)
				{
					// If we managed to wait, we can get the exit code immediately after
					DWORD dw_process_exit_code = 0;
					GetExitCodeProcess(m_process_handle, &dw_process_exit_code);

					if (dw_process_exit_code == 0 || dw_process_exit_code == CRSTL_STATUS_PENDING)
					{
						m_state = process_state::waited;
					}
					else
					{
						m_state = process_state::error_wait;
					}

					m_exit_code = process_exit_code((process_exit_code::t)dw_process_exit_code);
				}
				else
				{
					m_state = process_state::error_wait;
				}
			}

			return m_exit_code;
		}

		process_size read_stdout(char* buffer, size_t buffer_size)
		{
			crstl_assert_msg(buffer != nullptr, "Buffer is null");
			crstl_assert_msg(buffer_size > 0, "Invalid size");

			DWORD total_bytes_read = 0;

			if (m_state == process_state::launched || m_state == process_state::waited)
			{
				while (true)
				{
					DWORD bytes_read = 0;
					bool success = ReadFile(m_stdout_read_handle, buffer + total_bytes_read, (DWORD)(buffer_size - 1 - total_bytes_read), &bytes_read, nullptr);
					total_bytes_read += bytes_read;

					if (!success || bytes_read == 0 || total_bytes_read >= buffer_size - 1)
					{
						break;
					}
				}

				// Null terminate the buffer
				buffer[total_bytes_read] = '\0';

				return process_size(total_bytes_read);
			}

			return process_size();
		}

		bool terminate()
		{
			if (m_state == process_state::launched)
			{
				LONG killed_process_exit_code = 99;
				BOOL terminate_success = TerminateProcess(m_process_handle, killed_process_exit_code);
				m_state = process_state::terminated;
				m_process_handle = nullptr;
				return terminate_success == 0;
			}

			return false;
		}

	private:

		HANDLE m_process_handle;

		HANDLE m_stdout_read_handle; // Input handle that we read from
	};
};