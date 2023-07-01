#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#if defined(CRSTL_WINDOWS)
#include "crstl/platform/timer_win32.h"
#elif defined(CRSTL_LINUX) || defined(CRSTL_OSX)
#include "crstl/platform/timer_posix.h"
#endif

namespace crstl
{
	template<typename=void>
	struct timer_globals
	{
		static uint64_t TicksPerSecond;
	};

	template<>
	uint64_t timer_globals<>::TicksPerSecond = ticks_per_second();

	// This only gets initialized once. See https://youtu.be/xVT1y0xWgww?t=1320 for the trick
	static uint64_t TicksPerSecond = timer_globals<>::TicksPerSecond;

	class time
	{
	public:

		time() : m_ticks(0) {}

		explicit time(int64_t ticks) : m_ticks(ticks) {}

		int64_t ticks() const
		{
			return m_ticks;
		}

		static time now()
		{
			return time(current_ticks());
		}

		double seconds() const
		{
			return (double)m_ticks / (double)TicksPerSecond;
		}

		double milliseconds() const
		{
			return (double)m_ticks * 1000.0 / (double)TicksPerSecond;
		}

		double microseconds() const
		{
			return (double)m_ticks * 1000000.0 / (double)TicksPerSecond;
		}

		double frequency() const
		{
			return (double)TicksPerSecond / (double)m_ticks;
		}

		inline time operator + (const time& other) const
		{
			return time(m_ticks + other.m_ticks);
		}

		inline time operator - (const time& other) const
		{
			return time(m_ticks - other.m_ticks);
		}

		inline time& operator += (const time& other)
		{
			m_ticks += other.m_ticks; return *this;
		}

		inline time& operator -= (const time& other)
		{
			m_ticks -= other.m_ticks; return *this;
		}

		inline time operator * (const int64_t other) const
		{
			return time(m_ticks * other);
		}

		inline time& operator *= (const int64_t other)
		{
			m_ticks *= other; return *this;
		}

		inline time operator / (const int64_t other) const
		{
			return time(m_ticks / other);
		}

		inline time& operator /= (const int64_t other)
		{
			m_ticks /= other; return *this;
		}

		inline bool	operator > (const time& other) const
		{
			return m_ticks > other.m_ticks;
		}

		inline bool	operator < (const time& other) const
		{
			return m_ticks < other.m_ticks;
		}

		inline bool	operator >= (const time& other) const
		{
			return m_ticks >= other.m_ticks;
		}

		inline bool	operator <= (const time& other) const
		{
			return m_ticks <= other.m_ticks;
		}

		inline bool	operator == (const time& other) const
		{
			return m_ticks == other.m_ticks;
		}

		inline bool	operator != (const time& other) const
		{
			return m_ticks != other.m_ticks;
		}

	private:

		int64_t m_ticks;
	};

	class timer
	{
	public:

		timer()
		{
			reset();
		}

		void reset()
		{
			m_start = time();
		}

		void start()
		{
			m_start = time::now();
		}

		time elapsed() const
		{
			return time::now() - m_start;
		}

	private:

		time m_start;
	};

	typedef uint64_t clock_cycle_t;

	class cycle_timer
	{
	public:

		cycle_timer()
		{
			reset();
			start();
		}

		static clock_cycle_t now()
		{
			return begin_cycle_count();
		}

		void reset()
		{
			m_start = 0;
		}

		void start()
		{
			m_start = begin_cycle_count();
		}

		clock_cycle_t elapsed()
		{
			return end_cycle_count() - m_start;
		}

	private:

		clock_cycle_t m_start;
	};
}
