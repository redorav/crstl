#pragma once

#include <pthread.h>

crstl_module_export namespace crstl
{
	class critical_section final : public critical_section_base
	{
	public:

		critical_section()
		{
			// A recursive mutex maintains a lock counter, just like the Windows critical sections
			m_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
		}

		void enter()
		{
			pthread_mutex_lock(&m_mutex);
		}

		crstl_nodiscard bool try_enter()
		{
			return pthread_mutex_trylock(&m_mutex) == 0;
		}

		void leave()
		{
			pthread_mutex_unlock(&m_mutex);
		}

		~critical_section()
		{
			pthread_mutex_destroy(&m_mutex);
		}

	protected:

		pthread_mutex_t m_mutex;
	};
};