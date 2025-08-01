#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#if defined(CRSTL_OS_WINDOWS)
#include "crstl/platform/atomic_win32.h"
#elif defined(CRSTL_ARCH_ARM)
#include "crstl/platform/atomic_arm.h"
#elif defined(CRSTL_OS_LINUX) || defined(CRSTL_OS_MACOS)
#include "crstl/platform/atomic_clang_gcc.h"
#endif

// crstl::atomic
//
// Replacement for std::atomic
//

crstl_module_export namespace crstl
{
	// atomic_store: store value to variable pointed by target, and return its previous value

	inline int8_t atomic_store(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return (int8_t)crstl_atomic_store8(target, value);
	}

	inline int16_t atomic_store(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return (int16_t)crstl_atomic_store16(target, value);
	}

	inline int32_t atomic_store(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return (int32_t)crstl_atomic_store32(target, value);
	}

	inline int64_t atomic_store(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return (int64_t)crstl_atomic_store64(target, value);
	}

	// atomic_add: add value to variable pointed by target, and return the previous value of target

	inline int8_t atomic_add(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return (int8_t)crstl_atomic_add8(target, value);
	}

	inline int16_t atomic_add(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return (int16_t)crstl_atomic_add16(target, value);
	}

	inline int32_t atomic_add(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return (int32_t)crstl_atomic_add32(target, value);
	}

	inline int64_t atomic_add(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return (int64_t)crstl_atomic_add64(target, value);
	}

	// atomic_sub: subtract value from variable pointed by target, and return the previous value of target

	inline int8_t atomic_sub(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return (int8_t)crstl_atomic_sub8(target, value);
	}

	inline int16_t atomic_sub(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return (int16_t)crstl_atomic_sub16(target, value);
	}

	inline int32_t atomic_sub(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return (int32_t)crstl_atomic_sub32(target, value);
	}

	inline int64_t atomic_sub(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return (int64_t)crstl_atomic_sub64(target, value);
	}

	// atomic_and: AND value with variable pointed by target, and return the previous value of target

	inline int8_t atomic_and(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return (int8_t)crstl_atomic_and8(target, value);
	}

	inline int16_t atomic_and(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return (int16_t)crstl_atomic_and16(target, value);
	}

	inline int32_t atomic_and(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return (int32_t)crstl_atomic_and32(target, value);
	}

	inline int64_t atomic_and(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return (int64_t)crstl_atomic_and64(target, value);
	}

	// atomic_or: OR value with variable pointed by target, and return the previous value of target

	inline int8_t atomic_or(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return (int8_t)crstl_atomic_or8(target, value);
	}

	inline int16_t atomic_or(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return (int16_t)crstl_atomic_or16(target, value);
	}

	inline int32_t atomic_or(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return (int32_t)crstl_atomic_or32(target, value);
	}

	inline int64_t atomic_or(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return (int64_t)crstl_atomic_or64(target, value);
	}

	// atomic_xor: XOR value with variable pointed by target, and return the previous value of target

	inline int8_t atomic_xor(int8_t volatile* target, int8_t value) crstl_noexcept
	{
		return (int8_t)crstl_atomic_xor8(target, value);
	}

	inline int16_t atomic_xor(int16_t volatile* target, int16_t value) crstl_noexcept
	{
		return (int16_t)crstl_atomic_xor16(target, value);
	}

	inline int32_t atomic_xor(int32_t volatile* target, int32_t value) crstl_noexcept
	{
		return (int32_t)crstl_atomic_xor32(target, value);
	}

	inline int64_t atomic_xor(int64_t volatile* target, int64_t value) crstl_noexcept
	{
		return (int64_t)crstl_atomic_xor64(target, value);
	}

	template<size_t N> struct atomic_type {};

	template<> struct atomic_type<1> { typedef int8_t type; };
	template<> struct atomic_type<2> { typedef int16_t type; };
	template<> struct atomic_type<4> { typedef int32_t type; };
	template<> struct atomic_type<8> { typedef int64_t type; };

	template<typename T>
	struct atomic
	{
	public:

		typedef T value_type;

		typedef typename atomic_type<sizeof(value_type)>::type operation_type;

		atomic() : m_value(0) {}

		atomic(const value_type& value)
		{
			atomic_store((operation_type*)&m_value, value);
		}

		atomic& operator += (const value_type& value)
		{
			atomic_add((operation_type*)&m_value, value); return *this;
		}

		atomic& operator -= (const value_type& value)
		{
			atomic_sub((operation_type*)&m_value, value); return *this;
		}

		atomic& operator &= (const value_type& value)
		{
			atomic_and((operation_type*)&m_value, value); return *this;
		}

		atomic& operator |= (const value_type& value)
		{
			atomic_or((operation_type*)&m_value, value); return *this;
		}

		atomic& operator ^= (const value_type& value)
		{
			atomic_xor((operation_type*)& m_value, value); return *this;
		}

		// Pre-increment
		value_type operator ++ ()
		{
			return (value_type)atomic_add((operation_type*)&m_value, operation_type(1)) + 1;
		}

		// Post-increment
		value_type operator ++ (int)
		{
			return (value_type)atomic_add((operation_type*)&m_value, 1);
		}

		// Pre-decrement
		value_type operator -- ()
		{
			return (value_type)atomic_sub((operation_type*)&m_value, 1) - 1;
		}

		// Post-decrement
		value_type operator -- (int)
		{
			return (value_type)atomic_sub((operation_type*)&m_value, 1);
		}

		operator value_type() const
		{
			return m_value;
		}

	private:

		value_type m_value;
	};
};
