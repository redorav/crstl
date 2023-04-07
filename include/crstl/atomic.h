#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#if defined(CRSTL_WINDOWS)
#include "crstl/platform/atomic_win32.h"
#elif defined(CRSTL_ARM)
#include "crstl/platform/atomic_arm.h"
#elif defined(CRSTL_LINUX)
#include "crstl/platform/atomic_linux.h"
#elif defined(CRSTL_OSX)
#include "crstl/platform/atomic_osx.h"
#endif

// These freestanding functions used to be called atomic_store, add, etc
// but stdatomic.h seems to create defines for these names which is quite
// unfortunate

crstl_module_export namespace crstl
{
	// store_atomic: store value to variable pointed by target, and return its previous value

	inline int8_t store_atomic(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return crstl_atomic_store8(target, value);
	}

	inline int16_t store_atomic(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return crstl_atomic_store16(target, value);
	}

	inline int32_t store_atomic(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return crstl_atomic_store32(target, value);
	}

	inline int64_t store_atomic(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return crstl_atomic_store64(target, value);
	}

	// add_atomic: add value to variable pointed by target, and return the previous value of target

	inline int8_t add_atomic(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return crstl_atomic_add8(target, value);
	}

	inline int16_t add_atomic(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return crstl_atomic_add16(target, value);
	}

	inline int32_t add_atomic(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return crstl_atomic_add32(target, value);
	}

	inline int64_t add_atomic(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return crstl_atomic_add64(target, value);
	}

	// and_atomic: AND value with variable pointed by target, and return the previous value of target

	inline int8_t and_atomic(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return crstl_atomic_and8(target, value);
	}

	inline int16_t and_atomic(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return crstl_atomic_and16(target, value);
	}

	inline int32_t and_atomic(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return crstl_atomic_and32(target, value);
	}

	inline int64_t and_atomic(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return crstl_atomic_and64(target, value);
	}

	// or_atomic: OR value with variable pointed by target, and return the previous value of target

	inline int8_t or_atomic(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return crstl_atomic_or8(target, value);
	}

	inline int16_t or_atomic(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return crstl_atomic_or16(target, value);
	}

	inline int32_t or_atomic(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return crstl_atomic_or32(target, value);
	}

	inline int64_t or_atomic(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return crstl_atomic_or64(target, value);
	}

	// xor_atomic: XOR value with variable pointed by target, and return the previous value of target

	inline int8_t xor_atomic(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return crstl_atomic_xor8(target, value);
	}

	inline int16_t xor_atomic(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return crstl_atomic_xor16(target, value);
	}

	inline int32_t xor_atomic(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return crstl_atomic_xor32(target, value);
	}

	inline int64_t xor_atomic(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return crstl_atomic_xor64(target, value);
	}

	template<size_t N> struct atomic_type {};

	template<> struct atomic_type<1> { typedef int8_t type; };
	template<> struct atomic_type<2> { typedef int16_t type; };
	template<> struct atomic_type<4> { typedef int32_t type; };
	template<> struct atomic_type<8> { typedef int64_t type; };

	template<typename T>
	class atomic
	{
	public:

		typedef T value_type;

		typedef typename atomic_type<sizeof(value_type)>::type operation_type;

		atomic() : m_value(0) {}

		atomic(const value_type& value)
		{
			store_atomic((operation_type*)&m_value, value);
		}

		atomic& operator += (const value_type& value)
		{
			add_atomic((operation_type*)&m_value, value); return *this;
		}

		atomic& operator -= (const value_type& value)
		{
			add_atomic((operation_type*)&m_value, -(operation_type)value); return *this;
		}

		atomic& operator &= (const value_type& value)
		{
			and_atomic((operation_type*)&m_value, value); return *this;
		}

		atomic& operator |= (const value_type& value)
		{
			or_atomic((operation_type*)&m_value, value); return *this;
		}

		atomic& operator ^= (const value_type& value)
		{
			xor_atomic((operation_type*)& m_value, value); return *this;
		}

		// Pre-increment
		value_type operator ++ ()
		{
			return add_atomic((operation_type*)&m_value, 1) + 1;
		}

		// Post-increment
		value_type operator ++ (int)
		{
			return add_atomic((operation_type*)&m_value, 1);
		}

		// Pre-decrement
		value_type operator -- ()
		{
			return add_atomic((operation_type*)&m_value, -1) - 1;
		}

		// Post-decrement
		value_type operator -- (int)
		{
			return add_atomic((operation_type*)&m_value, -1);
		}

		operator value_type() const
		{
			return m_value;
		}

	protected:

		value_type m_value;
	};
};
