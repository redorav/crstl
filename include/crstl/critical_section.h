#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

// crstl::critical_section
//
// Implementation of critical section
//
// enter: request ownership of the critical section
// try_enter: attempt to enter critical section without blocking
// leave: release the critical section

crstl_module_export namespace crstl
{
	class critical_section_base
	{
	public:

		critical_section_base() {}

	private:

		critical_section_base(const critical_section_base& other) crstl_constructor_delete;

		critical_section_base& operator = (const critical_section_base& other) crstl_constructor_delete;
	};
};

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/critical_section_win32.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_ANDROID) || defined(CRSTL_OS_OSX)
#include "crstl/platform/critical_section_posix.h"
#endif

crstl_module_export namespace crstl
{
	class scoped_critical_section_lock
	{
	public:

		crstl_nodiscard
		scoped_critical_section_lock(critical_section& cs)
			: m_critical_section(cs)
		{
			m_critical_section.lock();
		}
		
		~scoped_critical_section_lock()
		{
			m_critical_section.unlock();
		}

	private:
		
		critical_section& m_critical_section;
	};
};