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

	struct process_exit_code
	{
		enum t : int
		{
			success                = 0,
			error_generic          = -1,
			error_failed_to_launch = 2147483647
		};

		process_exit_code() : return_code(error_failed_to_launch) {}

		process_exit_code(t return_code) : return_code(return_code) {}

		explicit operator bool() const { return return_code != error_failed_to_launch; }

		// This return code is process-specific, and should be handled by the application
		int get_exit_code()
		{
			return return_code;
		}

	private:

		t return_code;
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
		process_size(t result, size_t size) : m_result(result), m_size(size) {}

		explicit operator bool() const { return m_result != error; }

		size_t size() const { return m_size; }

	private:

		t m_result;

		size_t m_size;
	};

	struct process_parameters
	{
		process_parameters() 
		: command_line(nullptr)
		, args(nullptr)
		, read_stdout(true)
		, write_stdin(true)
			{}

		const char* command_line;
		const char* args;
		bool read_stdout;
		bool write_stdin;
	};

	class process_base
	{
	public:

		process_base()
			: m_state(process_state::undefined)
		{}

		process_exit_code get_return_code() const { return m_exit_code; }

	protected:

		process_state::t m_state;

		process_exit_code m_exit_code;

	private:

		process_base(const process_base& other) crstl_constructor_delete;

		process_base& operator = (const process_base& other) crstl_constructor_delete;
	};
}

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/process_win32.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_OSX)
#include "crstl/platform/process_posix.h"
#endif