#pragma once

#include "crstl/config.h"

crstl_module_export namespace crstl
{
	// Result of launching the process. Errors in the execution of the process
	// will be the responsibility of the return value when calling join()
	namespace process_state
	{
		enum t
		{
			undefined,    // Process launch has not been attempted yet
			launched,     // Process launched successfully
			terminated,   // Process was forcibly terminated
			joined,       // Process was gracefully waited to finish
			error_launch, // There was an error launching the process
			error_join,   // There was an error during join
		};
	};

	struct process_parameters
	{
		const char* command_line;
		const char* args;
		bool read_stdout = true;
		bool write_stdin = false;
	};

	class process_base
	{
	public:

		static const int kInvalidReturnValue = 2147483647;

		process_base()
			: m_state(process_state::undefined)
		{}

		process_state::t get_state() const { return m_state; }

	protected:

		process_state::t m_state;

	private:

		process_base(const process_base& other) crstl_constructor_delete;

		process_base& operator = (const process_base& other) crstl_constructor_delete;
	};
};

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/process_win32.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_OSX)
#include "crstl/platform/process_posix.h"
#endif