#pragma once

#include <pthread.h>

crstl_module_export namespace crstl
{
	class critical_section final : public critical_section_base
	{
	public:

		critical_section()
		{
			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);

			// A recursive mutex maintains a lock counter, just like the Windows critical sections
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&m_mutex, &attr);
			pthread_mutexattr_destroy(&attr);
		}

		void lock()
		{
			pthread_mutex_lock(&m_mutex);
		}

		crstl_nodiscard bool try_lock()
		{
			return pthread_mutex_trylock(&m_mutex) == 0;
		}

		void unlock()
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