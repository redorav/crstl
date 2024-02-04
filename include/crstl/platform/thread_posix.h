#pragma once

#include <pthread.h>

extern "C"
{
	int usleep(uint32_t __useconds);
}

crstl_module_export namespace crstl
{
	template<typename Function, typename ... Args>
	void* MainThreadFn(void* parameter)
	{
		crstl::thread_data<Function, Args...>* thread_data = (crstl::thread_data<Function, Args...>*)parameter;

		// If we want to delay the start, we need to signal a condition that this function has run
		if (thread_data->parameters.delay_start)
		{
			int result;
			result = pthread_mutex_lock(&thread_data->thread_ptr->m_mainthread_mutex);
			crstl_assert(result == 0);
			result = pthread_cond_wait(&thread_data->thread_ptr->m_mainthread_condition, &thread_data->thread_ptr->m_mainthread_mutex);
			crstl_assert(result == 0);
			result = pthread_mutex_unlock(&thread_data->thread_ptr->m_mainthread_mutex);
			crstl_assert(result == 0);
		}

		// TODO Do we need the result of the function
		call(thread_data->function, thread_data->function_args, crstl::make_index_sequence<sizeof...(Args)>());

		delete thread_data;

		pthread_exit(nullptr);

		return nullptr;
	}

	class thread final : thread_base
	{
	public:

		#define crstl_thread_invalid_handle 0

		thread() crstl_nodiscard : m_delayed_start(false), m_pthread(crstl_thread_invalid_handle) {}

		thread(thread&& other) crstl_nodiscard
		{
			m_pthread = other.m_pthread;
			m_delayed_start = other.m_delayed_start;

			other.m_pthread = crstl_thread_invalid_handle;
		}

		thread& operator = (thread&& other)
		{
			join();

			m_pthread = other.m_pthread;
			m_delayed_start = other.m_delayed_start;

			other.m_pthread = crstl_thread_invalid_handle;

			return *this;
		}

		template<typename Function, typename ... Args>
		thread(const thread_parameters& parameters, Function&& func, Args&& ... args) crstl_nodiscard
		{
			pthread_attr_t attr;
			pthread_attr_init(&attr);

			crstl::thread_data<Function, Args...>* thread_data = new crstl::thread_data<Function, Args...>
			{
				this,
				parameters,
				func,
				crstl::tuple<Args...>(crstl::forward<Args>(args)...)
			};

			m_delayed_start = parameters.delay_start;

			if (m_delayed_start)
			{
				int mutex_result = pthread_mutex_init(&m_mainthread_mutex, nullptr);
				crstl_assert(mutex_result == 0);
				int cond_result = pthread_cond_init(&m_mainthread_condition, nullptr);
				crstl_assert(cond_result == 0);
			}

			uint32_t stackSize = parameters.stack_size ? parameters.stack_size : DefaultStackSize;
			pthread_attr_setstacksize(&attr, stackSize);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			pthread_create(&m_pthread, &attr, MainThreadFn<Function, Args...>, thread_data);
			pthread_attr_destroy(&attr);

			m_started = !parameters.delay_start;

			set_priority(parameters.priority);
		}

		void set_priority(thread_priority::t priority)
		{
			if (m_priority != priority)
			{
				int highest_priority = sched_get_priority_max(SCHED_RR);
				int lowest_priority = sched_get_priority_min(SCHED_RR);
				int posix_priority = lowest_priority + ((highest_priority - lowest_priority) * static_cast<int>(priority)) / (thread_priority::count - 1);

				sched_param sched_parameter;
				sched_parameter.sched_priority = posix_priority;
				int result = pthread_setschedparam(m_pthread, SCHED_RR, &sched_parameter);

				if (result != 0)
				{
					m_priority = priority;
				}
			}
		}

		void start()
		{
			if (!m_started)
			{
				int result;
				result = pthread_mutex_lock(&m_mainthread_mutex);
				crstl_assert(result == 0);
				result = pthread_cond_signal(&m_mainthread_condition);
				crstl_assert(result == 0);
				result = pthread_mutex_unlock(&m_mainthread_mutex);
				crstl_assert(result == 0);

				m_started = true;
			}
		}

		void join()
		{
			if (m_pthread != crstl_thread_invalid_handle)
			{
				void* value_pointer = nullptr;
				int result = pthread_join(m_pthread, &value_pointer);
				crstl_assert(result == 0);
				m_pthread = crstl_thread_invalid_handle;
			}
		}

		~thread()
		{
			join();

			if (m_delayed_start)
			{
				pthread_mutex_destroy(&m_mainthread_mutex);
				pthread_cond_destroy(&m_mainthread_condition);
			}
		}

		bool m_delayed_start;

		pthread_mutex_t m_mainthread_mutex;

		pthread_cond_t m_mainthread_condition;

		pthread_t m_pthread;
	};

	namespace this_thread
	{
		// Yields timeslice back to the OS
		inline void yield()
		{
			sched_yield(); // pthread_yield is deprecated
		}

		inline void sleep_for(uint32_t milliseconds)
		{
			usleep(milliseconds * 1000);
		}
	};
};