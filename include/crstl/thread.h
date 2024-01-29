#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/tuple.h"

// crstl::thread
//
// Replacement for std::thread
//
// - Accepts a thread_parameters structure that configures the thread for initialization
//   This allows the implementation to set a thread name for debugging or an initial priority
// - There are extra functions not present in the standard such as
//   - set_priority: sets thread priority
//   - start: combine with with the delay_start parameter to start the thread at some other
//     point after creation
//

crstl_module_export namespace crstl
{
	const uint32_t DefaultStackSize = 32 * 1024;

	namespace thread_priority
	{
		enum t
		{
			idle         = 0,
			lowest       = 1,
			below_normal = 2,
			normal       = 3,
			abovenormal  = 4,
			highest      = 5,
			count
		};
	};

	struct thread_parameters
	{
		thread_parameters()
			: stack_size(0)
			, priority(thread_priority::normal)
			, core(-1)
			, delay_start(false)
			, debug_name(nullptr)
		{}

		// Size of stack in bytes
		uint32_t           stack_size;

		// Define initial priority
		thread_priority::t priority;
		
		// Peg thread to a core
		int32_t            core;

		// Start thread execution with resume(), otherwise it runs as soon as the thread is created
		bool               delay_start;

		// Name that appears in thread when debugging
		const char*        debug_name;
	};

	class thread;

	// Data we copy from the thread_parameters that needs to have the same lifetime as the thread
	template<typename Function, typename ... Args>
	struct thread_data
	{
		thread* thread_ptr;

		thread_parameters parameters;

		Function function;

		crstl::tuple<Args...> function_args;
	};

	// Call the Function with the Tuple as arguments. These come from the thread_data we create on construction of the thread
	template<typename Function, typename Tuple, size_t ... S>
	void call(Function&& function, Tuple&& arguments, crstl::index_sequence<S...>)
	{
		crstl_unused(arguments); // For some reason some compilers seem to think arguments is unused when the tuple is empty
		function(crstl::get<S>(arguments)...);
	}

	// Base class of thread. Ensures copy constructor is deleted
	class thread_base
	{
	public:

		thread_base()
		: m_started(false) 
		, m_priority(thread_priority::count) {}

	protected:

		bool m_started;

		uint32_t m_stack_bytes;

		thread_priority::t m_priority;

	private:

		thread_base(const thread_base& other) crstl_constructor_delete;

		thread_base& operator = (const thread_base& other) crstl_constructor_delete;
	};
};

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/thread_win32.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_ANDROID) || defined(CRSTL_OS_OSX)
#include "crstl/platform/thread_posix.h"
#endif

static_assert(crstl_is_base_of(crstl::thread_base, crstl::thread) && crstl_is_final(crstl::thread), "");