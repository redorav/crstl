#pragma once

#include "crstl/config.h"

crstl_module_export namespace crstl
{
	// State of the process. Errors in the execution of the process will be 
	// notified through the return value when calling wait()
	namespace process_state
	{
		enum t
		{
			undefined,              // Process launch has not been attempted yet
			launched,               // Process launched successfully
			terminated,             // Process was forcibly terminated
			waited,                 // Process was waited to completion. Check the return code
			error_failed_to_launch, // There was an error launching the process
			error_wait,             // There was an error during wait
		};
	}

	struct process_exit_status
	{
		enum class status
		{
			success,
			error
		};

		process_exit_status() : status(status::error), exit_code(2147483647) {}

		process_exit_status(int exit_code) : status(status::success), exit_code(exit_code) {}

		explicit operator bool() const { return status == status::success; }

		// This return code is process-specific, and should be handled by the application
		int get_exit_code()
		{
			return exit_code;
		}

	private:

		status status;

		int exit_code;
	};

	// Handles the state of a read or write operation. For example, a read on a process returns
	// how many bytes were read, but also whether there was an error during the operation
	struct process_size
	{
		enum t
		{
			success,
			error
		};

		process_size() : m_result(error), m_size(0) {}
		process_size(size_t size) : m_result(success), m_size(size) {}

		explicit operator bool() const { return m_result != error; }

		size_t size() const { return m_size; }

	private:

		t m_result;

		size_t m_size;
	};

	class process_base
	{
	public:

		process_base()
			: m_state(process_state::undefined)
		{}

		process_exit_status get_exit_status() const
		{
			return m_exit_status;
		}

		bool is_launched() const
		{
			return m_state == process_state::launched;
		}

	protected:

		process_state::t m_state;

		process_exit_status m_exit_status;

	private:

		process_base(const process_base& other) crstl_constructor_delete;

		process_base& operator = (const process_base& other) crstl_constructor_delete;
	};
}

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/process_win32.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_MACOS)
#include "crstl/platform/process_posix.h"
#endif