#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/platform/timer_platform.h"

// crstl::timer
//
// Replacement for std::chrono facilities
//

crstl_module_export namespace crstl
{
	template<typename Dummy>
	struct timer_globals { static const double TicksToSeconds; };

	// This only gets initialized once. See https://youtu.be/xVT1y0xWgww?t=1320 for the trick and
	// https://www.reddit.com/r/cpp/comments/e41t8r/stdnumeric_limits_members_functions_vs_constants/
	// for the variation that works across compilers
	template<typename Dummy>
	const double timer_globals<Dummy>::TicksToSeconds = ticks_to_seconds();

	class time
	{
	public:

		time() : m_ticks(0) {}

		explicit time(int64_t ticks) : m_ticks(ticks) {}

		// Return raw ticks value
		int64_t ticks() const
		{
			return m_ticks;
		}

		static time now()
		{
			return time(current_ticks());
		}

		// Return ticks as seconds
		double seconds() const
		{
			return (double)m_ticks * timer_globals<int64_t>::TicksToSeconds;
		}

		// Return ticks as milliseconds
		double milliseconds() const
		{
			return (double)m_ticks * 1000.0 * timer_globals<int64_t>::TicksToSeconds;
		}

		// Return ticks as microseconds
		double microseconds() const
		{
			return (double)m_ticks * 1000000.0 * timer_globals<int64_t>::TicksToSeconds;
		}

		// Return number of ticks per second
		double ticks_per_second() const
		{
			return 1.0 / ((double)m_ticks * timer_globals<int64_t>::TicksToSeconds);
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

		timer(bool immediate_start = true)
		{
			immediate_start ? start() : reset();
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
};
